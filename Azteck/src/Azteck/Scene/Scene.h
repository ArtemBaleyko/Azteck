#pragma once

#include <entt.hpp>

#include "Azteck/Core/Timestep.h"

namespace Azteck
{
	class Scene
	{
		friend class Entity;

	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = {});

		void onUpdate(Timestep ts);

	private:
		entt::registry _registry;
	};
}