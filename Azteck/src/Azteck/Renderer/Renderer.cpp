#include "azpch.h"
#include "Renderer.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"

namespace Azteck
{
	Scope<Renderer::SceneData> Renderer::_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init()
	{
		AZ_PROFILE_FUNCTION();

		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::shutdown()
	{
		Renderer2D::shutdown();
	}

	void Renderer::beginScene(const OrthographicCamera& camera)
	{
		_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{

	}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		shader->setMat4("u_ViewProjection", _sceneData->viewProjectionMatrix);
		shader->setMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}

	void Renderer::onWindowResized(uint32_t width, uint32_t height)
	{
		RenderCommand::setViewport(0, 0, width, height);
	}

}