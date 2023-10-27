#pragma once

#include "Azteck/Core/Core.h"
#include "Azteck/Scene/Scene.h"
#include "Azteck/Core/Log.h"
#include "Azteck/Scene/Entity.h"

namespace Azteck
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void setContext(const Ref<Scene>& context);

		void onImGuiRender();

	private:
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);

	private:
		Ref<Scene> _context;
		Entity _selectedEntity;
	};
}
