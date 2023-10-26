#pragma once

#include <entt.hpp>

#include "Scene.h"

namespace Azteck
{
	class Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			AZ_CORE_ASSERT(!hasComponent<T>(), "Entity already has the component");
			return _scene->_registry.emplace<T>(_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			AZ_CORE_ASSERT(hasComponent<T>(), "Entity doesn`t have the component");
			return _scene->_registry.get<T>(_handle);
		}

		template<typename T>
		void removeComponent()
		{
			AZ_CORE_ASSERT(hasComponent<T>(), "Entity doesn`t have the component");
			_scene->_registry.remove<T>(_handle);
		}

		template<typename T>
		bool hasComponent()
		{
			return _scene->_registry.any_of<T>(_handle);
		}

		operator bool() const { return _handle != entt::null; }

	private:
		entt::entity _handle;
		Scene* _scene;
	};
}