#pragma once

#include "RendererAPI.h"

namespace Azteck
{
	class RenderCommand
	{
	public:
		inline static void init()
		{
			_rendererAPI->init();
		}

		inline static void setClearColor(const glm::vec4& color)
		{
			_rendererAPI->setClearColor(color);
		}

		inline static void clear()
		{
			_rendererAPI->clear();
		}

		inline static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			_rendererAPI->drawIndexed(vertexArray, indexCount);
		}

		inline static void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			_rendererAPI->drawLines(vertexArray, vertexCount);
		}

		inline static void setLineWidth(float width)
		{
			_rendererAPI->setLineWidth(width);
		}

		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			_rendererAPI->setViewport(x, y, width, height);
		}

	private:
		static Scope<RendererAPI> _rendererAPI;
	};
}
