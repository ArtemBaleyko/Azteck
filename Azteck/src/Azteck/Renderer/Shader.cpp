#include "azpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Azteck
{
	Shader* Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				AZ_CORE_ASSERT(false, "RendererAPI::None is not supported");
				return nullptr;
			}

			case RendererAPI::API::OpenGL:
			{
				return new OpenGLShader(vertexSrc, fragmentSrc);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}
}