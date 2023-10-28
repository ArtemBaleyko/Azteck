#include "azpch.h"
#include "Scene.h"

#include "Azteck/Renderer/Renderer2D.h"
#include "Components.h"
#include "Entity.h"

namespace Azteck
{
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
		Entity entity{ _registry.create(), this };
		entity.addComponent<TransformComponent>();

		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::onUpdate(Timestep ts)
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

			auto group = _registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);
				Renderer2D::drawQuad(transform.getTransform(), sprite.color);
			}

			Renderer2D::endScene();
		}
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
}
