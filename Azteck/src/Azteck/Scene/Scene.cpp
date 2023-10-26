#include "azpch.h"
#include "Scene.h"

#include "Azteck/Renderer/Renderer2D.h"
#include "Components.h"
#include "Entity.h"

namespace Azteck
{
	Scene::Scene()
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
		auto group = _registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);

		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);
			Renderer2D::drawQuad(transform, sprite.color);
		}
	}
}