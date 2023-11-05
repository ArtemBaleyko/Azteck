#include "azpch.h"
#include "Scene.h"

#include "Azteck/Renderer/Renderer2D.h"
#include "Components.h"
#include "Entity.h"
#include "ScriptableEntity.h"

#include "Azteck/Scripting/ScriptEngine.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

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
		, _isRunning(false)
	{
	}

	Scene::~Scene()
	{
		delete _physicsWorld;
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

		_entityMap[uuid] = entity;

		return entity;
	}

	void Scene::destroyEntity(Entity entity)
	{
		_registry.destroy(entity);
		_entityMap.erase(entity.getUUID());
	}

	void Scene::onRuntimeStart()
	{
		_isRunning = true;

		onPhysics2DStart();

		ScriptEngine::onRuntimeStart(this);

		auto view = getAllEntitiesWith<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::onCreateEntity(entity);
		}
	}

	void Scene::onRuntimeStop()
	{
		_isRunning = false;

		ScriptEngine::onRuntimeStop();
		onPhysics2DStop();
	}

	void Scene::onSimulationStart()
	{
		onPhysics2DStart();
	}

	void Scene::onSimulationStop()
	{
		onPhysics2DStop();
	}

	void Scene::onUpdateRuntime(Timestep ts)
	{
		onUpdateScriptComponents(ts);
		onUpdateNativeScriptComponents(ts);
		onUpdatePhysics(ts);

		Entity primaryCameraEntity = getPrimaryCamera();

		if (primaryCameraEntity)
		{
			auto& cameraComponent = primaryCameraEntity.getComponent<CameraComponent>();
			auto& transformComponent = primaryCameraEntity.getComponent<TransformComponent>();

			Renderer2D::beginScene(cameraComponent.camera, transformComponent.getTransform());

			{
				auto view = getAllEntitiesWith<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::drawSprite(transform.getTransform(), sprite, static_cast<int>(entity));
				}
			}

			{
				auto view = getAllEntitiesWith<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, static_cast<int>(entity));
				}
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		onUpdatePhysics(ts);
		renderScene(camera);
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		renderScene(camera);
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		if (_viewportWidth == width && _viewportHeight == height)
			return;

		_viewportWidth = width;
		_viewportHeight = height;

		auto view = getAllEntitiesWith<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(width, height);
		}
	}

	Entity Scene::getPrimaryCamera()
	{
		auto view = getAllEntitiesWith<CameraComponent>();

		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity{ entity, this };
		}

		return {};
	}

	template<typename... Component>
	static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).id);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void copyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		copyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void copyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.hasComponent<Component>())
					dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void copyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		copyComponentIfExists<Component...>(dst, src);
	}

	void Scene::duplicateEntity(Entity entity)
	{
		const std::string& name = entity.getName();
		Entity newEntity = createEntity(name);

		copyComponentIfExists(AllComponents{}, newEntity, entity);
	}

	Entity Scene::getEntityByUUID(UUID uuid)
	{
		if (_entityMap.find(uuid) != _entityMap.end())
			return { _entityMap.at(uuid), this };

		return {};
	}

	Entity Scene::getEntityByName(std::string_view name)
	{
		auto view = getAllEntitiesWith<TagComponent>();

		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.tag == name)
				return Entity{ entity, this };
		}

		return {};
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

		copyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	void Scene::onPhysics2DStart()
	{
		_physicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = getAllEntitiesWith<Rigidbody2DComponent>();
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

			if (entity.hasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.getComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
				circleShape.m_radius = transform.scale.x * cc2d.radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.density;
				fixtureDef.friction = cc2d.friction;
				fixtureDef.restitution = cc2d.restitution;
				fixtureDef.restitutionThreshold = cc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::onPhysics2DStop()
	{
		delete _physicsWorld;
		_physicsWorld = nullptr;
	}

	void Scene::onUpdatePhysics(Timestep ts)
	{
		if (_physicsWorld)
		{
			const int32_t velocityIterations = 6;
			const int32_t positionsIterations = 2;

			_physicsWorld->Step(ts, velocityIterations, positionsIterations);

			// Get transform from Box2D
			auto view = getAllEntitiesWith<Rigidbody2DComponent>();
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
	}

	void Scene::onUpdateScriptComponents(Timestep ts)
	{
		auto view = getAllEntitiesWith<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::onUpdateEntity(entity, ts);
		}
	}

	void Scene::onUpdateNativeScriptComponents(Timestep ts)
	{
		getAllEntitiesWith<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc)
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
	}

	void Scene::renderScene(EditorCamera& camera)
	{
		Renderer2D::beginScene(camera);

		{
			auto view = getAllEntitiesWith<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawSprite(transform.getTransform(), sprite, static_cast<int>(entity));
			}
		}

		{
			auto view = getAllEntitiesWith<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, static_cast<int>(entity));
			}
		}

		Renderer2D::endScene();
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		// static_assert(false) compiles only with MSVC
		//static_assert(sizeof(T) == 0);
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
	void Scene::onComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
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

	template<>
	void Scene::onComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}
}
