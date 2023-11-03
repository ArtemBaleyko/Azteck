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

		void onScenePlay();
		void onSceneSimulate();
		void onSceneStop();		

		void onDuplicateEntity();

		void onOverlayRender();

		void newScene();
		void openScene();
		void openScene(const std::filesystem::path& filepath);
		void saveSceneAs();
		void saveScene();

		void serializeScene(const Ref<Scene>& scene, const std::filesystem::path& filepath);

		bool canSelectEntity();

		void uiDockspace();
		void uiToolbar();
		void uiMenubar();
		void uiSettings();
		void uiStats();
		void uiViewport();
		void uiGizmos();

		void renderColliders();
		void renderCircleColliders();
		void renderBoxColliders();
		void renderSelectedEntityOutline();

		Entity getHoveredEntity();

	private:
		glm::vec2 _viewportSize;
		glm::vec2 _viewportBounds[2];
		bool _isViewportFocused;
		bool _isViewportHovered;
		int _gizmoType;
		int _lastGizmoType;
		bool _showPhysicsColliders;

		EditorCamera _editorCamera;

		Ref<FrameBuffer> _frameBuffer;
		Ref<Scene> _activeScene;
		Ref<Scene> _editorScene;
		std::filesystem::path _editorScenePath;
		Entity _hoveredEntity;

		//Panels
		SceneHierarchyPanel _sceneHierarchyPanel;
		ContentBrowserPanel _contentBrowserPanel;

		enum class SceneState
		{
			Edit = 0,
			Play,
			Simulate
		};

		SceneState _sceneState = SceneState::Edit;
		Ref<Texture2D> _iconPlay;
		Ref<Texture2D> _iconStop;
		Ref<Texture2D> _iconSimulate;
	};
}
