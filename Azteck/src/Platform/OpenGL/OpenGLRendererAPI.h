#pragma once

#include "Azteck/Renderer/RendererAPI.h"

namespace Azteck
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void init() override;

		void setClearColor(const glm::vec4& color) override;
		void clear() override;

		void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}