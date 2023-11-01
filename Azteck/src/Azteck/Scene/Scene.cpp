#include "azpch.h"
#include "Scene.h"

#include "Azteck/Renderer/Renderer2D.h"
#include "Components.h"
#include "Entity.h"
#include "ScriptableEntity.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

namespace Azteck
{
	static b2BodyType toBox2DBodyType(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
			case Azteck::Rigidbody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case Azteck::Rigidbody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case Azteck::Rigidbody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
			default:
				break;
		}

		AZ_CORE_ASSERT(false, "Box2D doesn`t support provided body type - {0}", (int)type);
		return b2BodyType::b2_staticBody;
	}

	Scene::Scene()
		: _viewportWidth(0)
		, _viewportHeight(0)
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::createEntity(const std::string& name)
	{
		return createEntityWithUUID(UUID(), name);
	}

	Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity{ _registry.create(), this };
		entity.addComponent<IDComponent>(uuid);
		entity.addComponent<TransformComponent>();

		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::destroyEntity(Entity entity)
	{
		_registry.destroy(entity);
	}

	void Scene::onRuntimeStart()
	{
		_physicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = _registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };

			auto& transform = entity.getComponent<TransformComponent>();
			auto& rb2d = entity.getComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = toBox2DBodyType(rb2d.type);
			bodyDef.fixedRotation = rb2d.fixedRotation;
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;

			b2Body* body = _physicsWorld->CreateBody(&bodyDef);
			rb2d.runtimeBody = body;

			if (entity.hasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.getComponent<BoxCollider2DComponent>();

				b2PolygonShape shape;
				shape.SetAsBox(bc2d.size.x * transform.scale.x, bc2d.size.y * transform.scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.density = bc2d.density;
				fixtureDef.friction = bc2d.friction;
				fixtureDef.restitution = bc2d.restitution;
				fixtureDef.restitutionThreshold = bc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::onRuntimeStop()
	{
		delete _physicsWorld;
		_physicsWorld = nullptr;
	}

	void Scene::onUpdateRuntime(Timestep ts)
	{
		_registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc)
		{
			if (!nsc.instance)
			{
				nsc.instance = nsc.instantiateScript();
				AZ_CORE_ASSERT(nsc.instance, "Native script components is nto initialized!");

				nsc.instance->_entity = Entity{ entity, this };
				nsc.instance->onCreate();
			}

			nsc.instance->onUpdate(ts);
		});

		// Physics
		if (_physicsWorld)
		{
			const int32_t velocityIterations = 6;
			const int32_t positionsIterations = 2;

			_physicsWorld->Step(ts, velocityIterations, positionsIterations);

			// Get transform from Box2D
			auto view = _registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.getComponent<TransformComponent>();
				auto& rb2d = entity.getComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.runtimeBody;
				AZ_CORE_ASSERT(body != nullptr, "Box2D body is not valid");

				const auto& position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		}

		Camera* primaryCamera = nullptr;
		glm::mat4 cameraTransform(1.0f);

		auto view = _registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.primary)
			{
				primaryCamera = &camera.camera;
				cameraTransform = transform.getTransform();
				break;
			}
		}

		if (primaryCamera)
		{
			Renderer2D::beginScene(*primaryCamera, cameraTransform);

			auto group = _registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawQuad(transform.getTransform(), sprite.color);
			}

			auto view = _registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, static_cast<int>(entity));
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::beginScene(camera);

		auto group = _registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::drawSprite(transform.getTransform(), sprite, static_cast<int>(entity));
		}

		auto view = _registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : view)
		{
			auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
			Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, static_cast<int>(entity));
		}

		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		_viewportWidth = width;
		_viewportHeight = height;

		auto view = _registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(width, height);
		}
	}

	Entity Scene::getPrimaryCamera()
	{
		auto view = _registry.view<CameraComponent>();

		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity{ entity, this };
		}

		return {};
	}

	template<typename Component>
	static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).id;
			AZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "Unknown entity");
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void copyComponentIfExists(Entity dst, Entity src)
	{
		if (src.hasComponent<Component>())
			dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
	}

	void Scene::duplicateEntity(Entity entity)
	{
		const std::string& name = entity.getName();
		Entity newEntity = createEntity(name);

		copyComponentIfExists<TransformComponent>(newEntity, entity);
		copyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		copyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		copyComponentIfExists<CameraComponent>(newEntity, entity);
		copyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		copyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		copyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
	}

	Ref<Scene> Scene::copy(const Ref<Scene>& other)
	{
		Ref<Scene> newScene = createRef<Scene>();

		newScene->_viewportWidth = other->_viewportWidth;
		newScene->_viewportHeight = other->_viewportHeight;

		auto& srcSceneRegistry = other->_registry;
		auto& dstSceneRegistry = newScene->_registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).tag;
			Entity newEntity = newScene->createEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		copyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (_viewportWidth > 0 && _viewportHeight > 0)
			component.camera.setViewportSize(_viewportWidth, _viewportHeight);
	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
}
