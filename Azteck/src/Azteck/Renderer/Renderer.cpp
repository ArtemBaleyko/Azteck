#include "azpch.h"
#include "Renderer.h"
#include "Shader.h"

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

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->bind();
		shader->uploadUniformMat4("u_viewProjection", _sceneData->viewProjectionMatrix);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}