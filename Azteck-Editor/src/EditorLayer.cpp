#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include "glm/gtc/type_ptr.hpp"

#include "Azteck/Scene/SceneSerializer.h"

namespace Azteck
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
		, _squareColor(0.2f, 0.3f, 0.8f, 1.0f)
		, _isViewportFocused(false)
		, _isViewportHovered(false)
		, _cameraController(1280.0f / 720.0f, true)
	{

	}

	void EditorLayer::onAttach()
	{
		AZ_PROFILE_FUNCTION();

		_texture = Texture2D::create("assets/textures/checkerboard.png");

		FrameBufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;

		_frameBuffer = FrameBuffer::create(spec);
		_activeScene = createRef<Scene>();

		_entity = _activeScene->createEntity("Square");
		_entity.addComponent<SpriteRenderComponent>(glm::vec4(0.0f, 0.0f, 1.0, 1.0f));

		_cameraEntity = _activeScene->createEntity("Camera");
		_cameraEntity.addComponent<CameraComponent>();

		class CameraController : public ScriptableEntity
		{
		public:
			void onCreate()
			{
			}

			void onDestroy()
			{

			}

			void onUpdate(Timestep ts)
			{
				auto& translation = getComponent<TransformComponent>().translation;
				float speed = 5.0f;
				if (Input::isKeyPressed(Key::A))
					translation.x -= speed * ts;
				if (Input::isKeyPressed(Key::D))
					translation.x += speed * ts;
				if (Input::isKeyPressed(Key::W))
					translation.y += speed * ts;
				if (Input::isKeyPressed(Key::S))
					translation.y -= speed * ts;
			}
		};

		_cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();

		_sceneHierarchyPanel.setContext(_activeScene);
	}

	void EditorLayer::onDetach()
	{
		AZ_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep timestep)
	{
		AZ_PROFILE_FUNCTION();

		const FrameBufferSpecification& spec = _frameBuffer->getSpecification();
		if (_viewportSize.x > 0.0f && _viewportSize.y > 0.0f &&
			(spec.width != _viewportSize.x || spec.height != _viewportSize.y))
		{
			_frameBuffer->resize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
			_cameraController.onResize(_viewportSize.x, _viewportSize.y);

			_activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
		}

		if (_isViewportFocused)
			_cameraController.onUpdate(timestep);

		Renderer2D::resetStats();
		_frameBuffer->bind();
		RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		RenderCommand::clear();

		_activeScene->onUpdate(timestep);
		_frameBuffer->unbind();
	}

	void EditorLayer::onEvent(Event& e)
	{
		_cameraController.onEvent(e);
	}

	void EditorLayer::onImGuiRender()
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

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Application::getInstance().close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		_sceneHierarchyPanel.onImGuiRender();

		ImGui::Begin("Settings");

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Viewport");

		_isViewportFocused = ImGui::IsWindowFocused();
		_isViewportHovered = ImGui::IsWindowHovered();
		Application::getInstance().getImGuiLayer()->setBlockEvents(!_isViewportFocused || !_isViewportHovered);
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		_viewportSize = { viewportSize.x, viewportSize.y };

		uint64_t textureID = _frameBuffer->getColorAttachmentRendererId();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
