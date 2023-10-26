#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include "glm/gtc/type_ptr.hpp"

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
	}

	void EditorLayer::onDetach()
	{
		AZ_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep timestep)
	{
		AZ_PROFILE_FUNCTION();

		if (_isViewportFocused)
			_cameraController.onUpdate(timestep);

		Renderer2D::resetStats();

		_frameBuffer->bind();

		RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		RenderCommand::clear();

		Renderer2D::beginScene(_cameraController.getCamera());
		Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, _squareColor);
		Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.9f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Renderer2D::drawQuad(glm::vec3(0.0f), { 10.0f, 10.0f }, _texture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		Renderer2D::drawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 6.0f, 6.0f }, 45.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
		Renderer2D::endScene();

		Renderer2D::beginScene(_cameraController.getCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Renderer2D::drawQuad({ x, y, 0.1f }, { 0.45f, 0.45f }, color);
			}
		}

		Renderer2D::endScene();
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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

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

		ImGui::Begin("Settings");

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(_squareColor));
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Viewport");

		_isViewportFocused = ImGui::IsWindowFocused();
		_isViewportHovered = ImGui::IsWindowHovered();
		Application::getInstance().getImGuiLayer()->setBlockEvents(!_isViewportFocused || !_isViewportHovered);
		

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		// TODO: make it more safe
		if (_viewportSize != *(glm::vec2*)&viewportSize)
		{
			_viewportSize = { viewportSize.x, viewportSize.y };
			_frameBuffer->resize(static_cast<uint32_t>(_viewportSize.x), static_cast<uint32_t>(_viewportSize.y));
			_cameraController.onResize(_viewportSize.x, _viewportSize.y);
		}

		uint32_t textureID = _frameBuffer->getColorAttachmentRendererId();
		ImGui::Image((void*)textureID, viewportSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
