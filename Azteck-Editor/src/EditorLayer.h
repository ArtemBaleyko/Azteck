#pragma once

#include <Azteck.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Azteck/Renderer/EditorCamera.h"

namespace Azteck
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void onAttach() override;
		void onDetach() override;

		void onUpdate(Timestep timestep) override;
		void onEvent(Event& e) override;
		void onImGuiRender() override;

	private:
		bool onKeyPressed(KeyPressedEvent& e);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);

		void newScene();
		void openScene();
		void openScene(const std::filesystem::path& filepath);
		void saveSceneAs();

		bool canSelectEntity();

	private:
		glm::vec2 _viewportSize;
		glm::vec2 _viewportBounds[2];
		bool _isViewportFocused;
		bool _isViewportHovered;
		int _gizmoType;

		EditorCamera _editorCamera;

		Ref<FrameBuffer> _frameBuffer;
		Ref<Scene> _activeScene;
		Entity _hoveredEntity;

		//Panels
		SceneHierarchyPanel _sceneHierarchyPanel;
		ContentBrowserPanel _contentBrowserPanel;
	};
}
