#pragma once

#include "RendererAPI.h"

namespace Azteck
{
	class RenderCommand
	{
	public:
		inline static void setClearColor(const glm::vec4& color)
		{
			_rendererAPI->setClearColor(color);
		}

		inline static void clear()
		{
			_rendererAPI->clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			_rendererAPI->DrawIndexed(vertexArray);
		}

	private:
		static RendererAPI* _rendererAPI;
	};
}