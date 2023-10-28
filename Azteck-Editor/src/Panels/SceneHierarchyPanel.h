#pragma once

#include <glm/glm.hpp>

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

		static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	private:
		Ref<Scene> _context;
		Entity _selectedEntity;
	};
}
