#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

#include "Azteck/Scene/SceneSerializer.h"
#include "Azteck/Utils/PlatformUtils.h"
#include "Azteck/Math/Math.h"
#include "Azteck/Scripting/ScriptEngine.h"

namespace Azteck
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
		, _isViewportFocused(false)
		, _isViewportHovered(false)
		, _gizmoType(ImGuizmo::OPERATION::TRANSLATE)
		, _lastGizmoType(ImGuizmo::OPERATION::TRANSLATE)
		, _showPhysicsColliders(false)
	{

	}

	void EditorLayer::onAttach()
	{
		AZ_PROFILE_FUNCTION();

		_iconPlay = Texture2D::create("resources/icons/PlayButton.png");
		_iconStop = Texture2D::create("resources/icons/StopButton.png");
		_iconSimulate = Texture2D::create("resources/icons/SimulateButton.png");
		_iconPause = Texture2D::create("resources/icons/PauseButton.png");
		_iconStep = Texture2D::create("resources/icons/StepButton.png");

		FrameBufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;
		spec.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };

		_frameBuffer = FrameBuffer::create(spec);

		_editorScene = createRef<Scene>();
		_activeScene = _editorScene;

		_sceneHierarchyPanel.setContext(_activeScene);
		_editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		auto commandLineArgs = Application::getInstance().getSpecification().args;
		if (commandLineArgs.count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			openProject(projectFilePath);
		}
		else
		{
			newProject();
		}
	}

	void EditorLayer::onDetach()
	{
		AZ_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep timestep)
	{
		AZ_PROFILE_FUNCTION();

		_activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);

		const FrameBufferSpecification& spec = _frameBuffer->getSpecification();

		if (_viewportSize.x > 0.0f && _viewportSize.y > 0.0f &&
			(spec.width != _viewportSize.x || spec.height != _viewportSize.y))
		{
			_frameBuffer->resize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
			_editorCamera.setViewportSize(_viewportSize.x, _viewportSize.y);
		}

		Renderer2D::resetStats();
		_frameBuffer->bind();
		RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		RenderCommand::clear();

		_frameBuffer->clearAttachment(1, -1);

		switch (_sceneState)
		{
			case SceneState::Edit:
			{
				_editorCamera.onUpdate(timestep);
				_activeScene->onUpdateEditor(timestep, _editorCamera);
				break;
			}
			case SceneState::Simulate:
			{
				_editorCamera.onUpdate(timestep);
				_activeScene->onUpdateSimulation(timestep, _editorCamera);
				break;
			}
			case SceneState::Play:
			{
				_activeScene->onUpdateRuntime(timestep);
				break;
			}
		}

		onOverlayRender();

		_hoveredEntity = getHoveredEntity();
		_frameBuffer->unbind();
	}

	void EditorLayer::onEvent(Event& e)
	{
		if (_sceneState == SceneState::Edit)
			_editorCamera.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(AZ_BIND_EVENT_FN(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(AZ_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
	}

	void EditorLayer::onImGuiRender()
	{
		uiDockspace();
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& e)
	{
		if (e.isRepeat())
			return false;

		bool isControl = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
		bool isShift = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);

		switch (e.getKeyCode())
		{
			case Key::S:
			{
				if (isControl)
					isShift ? saveSceneAs() : saveScene();
				break;
			}
			case Key::N:
			{
				if (isControl)
					newScene();
				break;
			}
			case Key::O:
			{
				if (isControl)
					openScene();
				break;
			}
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing())
					_gizmoType = -1;
				break;
			}
			case Key::D:
			{
				if (isControl)
					onDuplicateEntity();
				break;
			}
			case Key::W:
			{
				if (!ImGuizmo::IsUsing() && !_activeScene->isRunning())
					_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing() && !_activeScene->isRunning())
					_gizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R:
			{
				if (isControl)
					ScriptEngine::reloadAssembly();
				else if (!ImGuizmo::IsUsing() && !_activeScene->isRunning())
					_gizmoType = ImGuizmo::OPERATION::SCALE;

				break;
			}
			case Key::Delete:
			{
				if (Entity selectedEntity = _sceneHierarchyPanel.getSelectedEntity())
				{
					_activeScene->destroyEntity(selectedEntity);
					_sceneHierarchyPanel.clearSelection();
				}
				break;
			}
			default:
				break;
		}

		return false;
	}

	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.getMouseButton() == Mouse::ButtonLeft && canSelectEntity())
		{
			_sceneHierarchyPanel.setSelectedEntity(_hoveredEntity);
		}

		return false;
	}

	void EditorLayer::onScenePlay()
	{
		if (_sceneState == SceneState::Simulate)
			onSceneStop();

		_sceneState = SceneState::Play;

		_lastGizmoType = _gizmoType;
		_gizmoType = -1;

		_activeScene = Scene::copy(_editorScene);
		_activeScene->onRuntimeStart();

		_sceneHierarchyPanel.setContext(_activeScene);
	}

	void EditorLayer::onSceneSimulate()
	{
		if (_sceneState == SceneState::Play)
			onSceneStop();

		_sceneState = SceneState::Simulate;

		_activeScene = Scene::copy(_editorScene);
		_activeScene->onSimulationStart();

		_sceneHierarchyPanel.setContext(_activeScene);
	}

	void EditorLayer::onSceneStop()
	{
		AZ_CORE_ASSERT(_sceneState == SceneState::Play || _sceneState == SceneState::Simulate, "Wrong scene state");

		if (_sceneState == SceneState::Play)
			_activeScene->onRuntimeStop();
		else if (_sceneState == SceneState::Simulate)
			_activeScene->onSimulationStop();

		_sceneState = SceneState::Edit;
		_gizmoType = _lastGizmoType;
		_activeScene = _editorScene;
		_sceneHierarchyPanel.setContext(_activeScene);
	}

	void EditorLayer::onDuplicateEntity()
	{
		if (_sceneState != SceneState::Edit)
			return;

		if (Entity selectedEntity = _sceneHierarchyPanel.getSelectedEntity())
			_editorScene->duplicateEntity(selectedEntity);
	}

	void EditorLayer::onOverlayRender()
	{
		if (_sceneState == SceneState::Play)
		{
			Entity camera = _activeScene->getPrimaryCamera();
			if (!camera)
				return;

			Renderer2D::beginScene(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>().getTransform());
		}
		else
		{
			Renderer2D::beginScene(_editorCamera);
		}

		if (_showPhysicsColliders)
			renderColliders();

		renderSelectedEntityOutline();

		Renderer2D::endScene();
	}

	void EditorLayer::newProject()
	{
		Project::create();
	}

	void EditorLayer::openProject(const std::filesystem::path& path)
	{
		if (Project::load(path))
		{
			auto startScenePath = Project::getAssetFileSystemPath(Project::getActive()->getConfig().startScenePath);
			openScene(startScenePath);

			_contentBrowserPanel = createScope<ContentBrowserPanel>();
		}
	}

	void EditorLayer::saveProject()
	{
	}

	void EditorLayer::newScene()
	{
		_editorScene = createRef<Scene>();
		//_editorScene->onViewportResize(static_cast<uint32_t>(_viewportSize.x), static_cast<uint32_t>(_viewportSize.y));
		_sceneHierarchyPanel.setContext(_editorScene);

		_activeScene = _editorScene;
		_editorScenePath.clear();
	}

	void EditorLayer::openScene()
	{
		std::optional<std::string> filepath = FileDialogs::openFile("Azteck Scene (*.yaml)\0*.yaml\0");
		if (filepath.has_value())
			openScene(filepath.value());
	}

	void EditorLayer::openScene(const std::filesystem::path& filepath)
	{
		if (_sceneState != SceneState::Edit)
			onSceneStop();

		if (filepath.extension().string() != ".yaml")
		{
			AZ_WARN("Could not load {0} - not a scene file", filepath.filename().string());
			return;
		}

		Ref<Scene> newScene = createRef<Scene>();
		SceneSerializer serializer(newScene);	
		if (serializer.deserialize(filepath.string()))
		{
			_editorScene = newScene;
			//_editorScene->onViewportResize(static_cast<uint32_t>(_viewportSize.x), static_cast<uint32_t>(_viewportSize.y));
			_sceneHierarchyPanel.setContext(_editorScene);

			_activeScene = _editorScene;
			_editorScenePath = filepath;
		}
	}

	void EditorLayer::saveSceneAs()
	{
		std::optional<std::string> filepath = FileDialogs::saveFile("Azteck Scene (*.yaml)\0*.yaml\0");
		if (filepath.has_value())
		{
			serializeScene(_activeScene, filepath.value());
			_editorScenePath = filepath.value();
		}
	}

	void EditorLayer::saveScene()
	{
		if (!_editorScenePath.empty())
			serializeScene(_activeScene, _editorScenePath);
		else
			saveSceneAs();
	}

	void EditorLayer::serializeScene(const Ref<Scene>& scene,  const std::filesystem::path& filepath)
	{
		SceneSerializer serializer(scene);
		serializer.serialize(filepath.string());
	}

	bool EditorLayer::canSelectEntity()
	{
		return _isViewportHovered && !ImGuizmo::IsOver() && !Input::isKeyPressed(Key::LeftAlt);
	}

	void EditorLayer::uiDockspace()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWindowSizeX;


		_sceneHierarchyPanel.onImGuiRender();
		_contentBrowserPanel->onImGuiRender();

		uiMenubar();
		uiStats();
		uiSettings();
		uiViewport();
		uiToolbar();

		ImGui::End();
	}

	void EditorLayer::uiToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		auto& colors = ImGui::GetStyle().Colors;

		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = _activeScene != nullptr;

		ImVec4 tintColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		const float size = ImGui::GetWindowHeight() - 4.0f;
		const bool isEditOrSimulateState = _sceneState == SceneState::Edit || _sceneState == SceneState::Simulate;
		const bool isEditOrPlayState = _sceneState == SceneState::Edit || _sceneState == SceneState::Play;

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if (isEditOrPlayState)
		{
			Ref<Texture2D> icon = isEditOrSimulateState ? _iconPlay : _iconStop;
			if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (isEditOrSimulateState)
					onScenePlay();
				else if (_sceneState == SceneState::Play)
					onSceneStop();
			}
		}

		if (isEditOrSimulateState)
		{
			if (isEditOrPlayState)
				ImGui::SameLine();

			Ref<Texture2D> icon = isEditOrPlayState ? _iconSimulate : _iconStop;
			if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (isEditOrPlayState)
					onSceneSimulate();
				else if (_sceneState == SceneState::Simulate)
					onSceneStop();
			}
		}

		if (_sceneState != SceneState::Edit)
		{
			bool isPaused = _activeScene->isPaused();
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = isPaused ? _iconPlay : _iconPause;
				if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					_activeScene->setPaused(!isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				{
					if (ImGui::ImageButton((ImTextureID)_iconStep->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						_activeScene->step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::uiMenubar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					newScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					openScene();

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					saveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					saveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) 
					Application::getInstance().close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
					ScriptEngine::reloadAssembly();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::uiSettings()
	{
		ImGui::Begin("Settings");
		ImGui::Checkbox("Show physics colliders", &_showPhysicsColliders);
		ImGui::End();
	}

	void EditorLayer::uiStats()
	{
		ImGui::Begin("Stats");

		std::string name = "None";
		if (_hoveredEntity)
			name = _hoveredEntity.getComponent<TagComponent>().tag;

		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();
	}

	void EditorLayer::uiViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		_isViewportFocused = ImGui::IsWindowFocused();
		_isViewportHovered = ImGui::IsWindowHovered();
		Application::getInstance().getImGuiLayer()->setBlockEvents(!_isViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		_viewportSize = { viewportSize.x, viewportSize.y };

		uint64_t textureID = _frameBuffer->getColorAttachmentRendererId();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				openScene(std::filesystem::path("assets") / path);
			}
			ImGui::EndDragDropTarget();
		}

		uiGizmos();


		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::uiGizmos()
	{
		Entity selectedEntity = _sceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && _gizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			const float windowWidth = ImGui::GetWindowWidth();
			const float windowHeight = ImGui::GetWindowHeight();
			ImGuizmo::SetRect(_viewportBounds[0].x, _viewportBounds[0].y, _viewportBounds[1].x - _viewportBounds[0].x, _viewportBounds[1].y - _viewportBounds[0].y);

			// Runtime camera fro entity
			//auto cameraEntity = _activeScene->getPrimaryCamera();
			//const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
			//const glm::mat4& cameraProjection = camera.getProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

			// Editor camera
			const glm::mat4& cameraProjection = _editorCamera.getProjection();
			glm::mat4 cameraView = _editorCamera.getViewMatrix();

			auto& transformComponent = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = transformComponent.getTransform();

			// Snapping
			bool snap = Input::isKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;

			if (_gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(_gizmoType), ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation;
				glm::vec3 rotation;
				glm::vec3 scale;
				if (Math::decomposeTransform(transform, translation, rotation, scale))
				{
					transformComponent.translation = translation;
					transformComponent.rotation = rotation;
					transformComponent.scale = scale;
				}

			}
		}
	}

	void EditorLayer::renderColliders()
	{
		renderBoxColliders();
		renderCircleColliders();
	}

	void EditorLayer::renderCircleColliders()
	{
		auto view = _activeScene->getAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
		for (auto entity : view)
		{
			auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

			glm::vec3 translation = tc.translation + glm::vec3(cc2d.offset, 0.001f);
			glm::vec3 scale = tc.scale * glm::vec3(cc2d.radius * 2.0f);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
				* glm::scale(glm::mat4(1.0f), scale);

			Renderer2D::drawCircle(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.05f, 0.005f);
		}
	}

	void EditorLayer::renderBoxColliders()
	{
		auto view = _activeScene->getAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
		for (auto entity : view)
		{
			auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

			glm::vec3 translation = tc.translation + glm::vec3(bc2d.offset, 0.001f);
			glm::vec3 scale = tc.scale * glm::vec3(bc2d.size * 2.0f, 1.0f);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
				* glm::rotate(glm::mat4(1.0f), tc.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), scale);

			Renderer2D::drawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	void EditorLayer::renderSelectedEntityOutline()
	{
		if (Entity selectedEntity = _sceneHierarchyPanel.getSelectedEntity()) {
			const TransformComponent& transform = selectedEntity.getComponent<TransformComponent>();

			Renderer2D::drawRect(transform.getTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}
	}

	Entity EditorLayer::getHoveredEntity()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= _viewportBounds[0].x;
		my -= _viewportBounds[0].y;
		my = _viewportSize.y - my;
		glm::vec2 viewportSize = _viewportBounds[1] - _viewportBounds[0];

		int mouseX = static_cast<int>(mx);
		int mouseY = static_cast<int>(my);

		Entity entity{};

		if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(_viewportSize.x) && mouseY < static_cast<int>(_viewportSize.y))
		{
			int pixelData = _frameBuffer->readPixel(1, mouseX, mouseY);
			entity = pixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(pixelData), _activeScene.get());
		}

		return entity;
	}
}
