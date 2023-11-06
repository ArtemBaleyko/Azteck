#pragma once

#include <entt.hpp>

#include "Azteck/Core/Timestep.h"
#include "Azteck/Core/UUID.h"
#include "Azteck/Renderer/EditorCamera.h"

class b2World;

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
		Entity createEntityWithUUID(UUID uuid, const std::string& name = {});
		void destroyEntity(Entity entity);

		void onRuntimeStart();
		void onRuntimeStop();

		void onSimulationStart();
		void onSimulationStop();

		bool isRunning() const { return _isRunning; }
		bool isPaused() const { return _isPaused; }

		void setPaused(bool paused) { _isPaused = paused; }

		void step(int frames = 1);

		void onUpdateRuntime(Timestep ts);
		void onUpdateSimulation(Timestep ts, EditorCamera& camera);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);
		void onViewportResize(uint32_t width, uint32_t height);

		Entity getPrimaryCamera();

		void duplicateEntity(Entity entity);

		Entity getEntityByUUID(UUID uuid);
		Entity getEntityByName(std::string_view name);

		template<typename... Components>
		auto getAllEntitiesWith()
		{
			return _registry.view<Components...>();
		}

		static Ref<Scene> copy(const Ref<Scene>& other);

	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);

		void onPhysics2DStart();
		void onPhysics2DStop();

		void onUpdatePhysics(Timestep ts);
		void onUpdateScriptComponents(Timestep ts);
		void onUpdateNativeScriptComponents(Timestep ts);

		void renderScene(EditorCamera& camera);

	private:
		entt::registry _registry;

		uint32_t _viewportWidth;
		uint32_t _viewportHeight;

		b2World* _physicsWorld = nullptr;

		bool _isRunning;
		bool _isPaused;

		int _stepFrames;

		std::unordered_map<UUID, entt::entity> _entityMap;
	};
}
