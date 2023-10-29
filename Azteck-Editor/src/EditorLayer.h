#pragma once

#include <Azteck.h>

#include "Panels/SceneHierarchyPanel.h"

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

		void newScene();
		void openScene();
		void saveSceneAs();

	private:
		OrthographicCameraController _cameraController;
		glm::vec2 _viewportSize;
		bool _isViewportFocused;
		bool _isViewportHovered;
		int _gizmoType;

		// Temporary
		Ref<Texture2D> _texture;
		Ref<FrameBuffer> _frameBuffer;
		glm::vec4 _squareColor;

		Ref<Scene> _activeScene;
		Entity _entity;
		Entity _cameraEntity;

		//Panels
		SceneHierarchyPanel _sceneHierarchyPanel;
	};
}
