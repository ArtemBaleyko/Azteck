#include "azpch.h"
#include "Renderer.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Azteck
{
	Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

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

		auto openGLShader = std::dynamic_pointer_cast<OpenGLShader>(shader);
		openGLShader->uploadUniformMat4("u_ViewProjection", _sceneData->viewProjectionMatrix);
		openGLShader->uploadUniformMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}