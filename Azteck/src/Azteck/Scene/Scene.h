#pragma once

#include <entt.hpp>

#include "Azteck/Core/Timestep.h"

namespace Azteck
{
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;

	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = {});
		void destroyEntity(Entity entity);

		void onUpdate(Timestep ts);
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);

	private:
		entt::registry _registry;

		uint32_t _viewportWidth;
		uint32_t _viewportHeight;
	};
}
