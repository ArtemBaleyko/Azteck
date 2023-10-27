#pragma once

#include "Entity.h"

namespace Azteck
{
	class ScriptableEntity
	{
		friend class Scene;

	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& getComponent()
		{
			return _entity.getComponent<T>();
		}

	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep ts) {}

	private:
		Entity _entity;
	};
}