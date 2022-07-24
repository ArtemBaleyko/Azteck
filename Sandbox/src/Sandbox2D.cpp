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

	Azteck::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	Azteck::RenderCommand::clear();

	Azteck::Renderer2D::beginScene(_cameraController.getCamera());

	Azteck::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, _squareColor);
	Azteck::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.9f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	Azteck::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, _texture, 10.0f, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));

	Azteck::Renderer2D::endScene();
}

void Sandbox2D::onEvent(Azteck::Event& e)
{
	_cameraController.onEvent(e);
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Color", glm::value_ptr(_squareColor));
	ImGui::End();
}
