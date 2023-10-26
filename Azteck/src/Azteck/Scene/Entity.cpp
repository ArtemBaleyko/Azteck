#include "azpch.h"
#include "Entity.h"

namespace Azteck
{
	Entity::Entity()
		: _handle{entt::null}
		, _scene{nullptr}
	{
	}

	Entity::Entity(entt::entity handle, Scene* scene)
		: _handle(handle)
		, _scene(scene)
	{
	}
}