#pragma once

#include "Azteck/Renderer/RendererAPI.h"

namespace Azteck
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}