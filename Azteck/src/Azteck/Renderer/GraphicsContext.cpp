#include "azpch.h"
#include "GraphicsContext.h"

#include "Azteck/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Azteck
{
	Scope<GraphicsContext> GraphicsContext::create(void* window)
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
				return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}
}