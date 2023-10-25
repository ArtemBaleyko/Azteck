#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include "glm/gtc/type_ptr.hpp"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, _squareColor(0.2f, 0.3f, 0.8f, 1.0f)
	, _cameraController(1280.0f / 720.0f, true)
{

}

void Sandbox2D::onAttach()
{
	AZ_PROFILE_FUNCTION();

	_texture = Azteck::Texture2D::create("assets/textures/checkerboard.png");
}

void Sandbox2D::onDetach()
{
	AZ_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Azteck::Timestep timestep)
{
	AZ_PROFILE_FUNCTION();

	_cameraController.onUpdate(timestep);

	Azteck::Renderer2D::resetStats();

	Azteck::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	Azteck::RenderCommand::clear();

	Azteck::Renderer2D::beginScene(_cameraController.getCamera());
	Azteck::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, _squareColor);
	Azteck::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.9f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	Azteck::Renderer2D::drawQuad(glm::vec3(0.0f), {10.0f, 10.0f}, _texture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
	Azteck::Renderer2D::drawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 6.0f, 6.0f }, 45.0f, {0.0f, 1.0f, 0.0f, 1.0f});
	Azteck::Renderer2D::endScene();

	Azteck::Renderer2D::beginScene(_cameraController.getCamera());

	for (float y = -5.0f; y < 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x < 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};
			Azteck::Renderer2D::drawQuad({ x, y, 0.1f }, { 0.45f, 0.45f }, color);
		}
	}

	Azteck::Renderer2D::endScene();
}

void Sandbox2D::onEvent(Azteck::Event& e)
{
	_cameraController.onEvent(e);
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");

	auto stats = Azteck::Renderer2D::getStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(_squareColor));
	ImGui::End();
}
