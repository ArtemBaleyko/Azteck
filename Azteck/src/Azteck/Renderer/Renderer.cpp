#include "azpch.h"
#include "Renderer.h"

namespace Azteck
{
	void Renderer::beginScene()
	{

	}

	void Renderer::endScene()
	{

	}

	void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}