#pragma once

#include <entt.hpp>

#include "Azteck/Core/Timestep.h"
#include "Azteck/Renderer/EditorCamera.h"

namespace Azteck
{
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;

	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = {});
		void destroyEntity(Entity entity);

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);
		void onViewportResize(uint32_t width, uint32_t height);

		Entity getPrimaryCamera();
	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);

	private:
		entt::registry _registry;

		uint32_t _viewportWidth;
		uint32_t _viewportHeight;
	};
}
