#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Azteck
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL
		};

	public:
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API getAPI() { return _api; }
	private:
		static API _api;
	};
}