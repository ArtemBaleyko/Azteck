#pragma once

#include <entt.hpp>

#include "Azteck/Core/UUID.h"
#include "Scene.h"
#include "Components.h"

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
			T& component = _scene->_registry.emplace<T>(_handle, std::forward<Args>(args)...);
			_scene->onComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args)
		{
			T& component = _scene->_registry.emplace_or_replace<T>(_handle, std::forward<Args>(args)...);
			_scene->onComponentAdded<T>(*this, component);
			return component;
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

		UUID getUUID() { return getComponent<IDComponent>().id; }
		const std::string& getName() { return getComponent<TagComponent>().tag; }

		operator bool() const { return _handle != entt::null; }
		operator uint32_t() const { return static_cast<uint32_t>(_handle); }
		operator entt::entity() const { return _handle; }

		bool operator==(const Entity& other) const { return _handle == other._handle && _scene == other._scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity _handle;
		Scene* _scene;
	};
}
