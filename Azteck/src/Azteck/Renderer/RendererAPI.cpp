#include "azpch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Azteck
{
	RendererAPI::API RendererAPI::_api = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::create()
	{
		switch (_api)
		{
			case RendererAPI::API::None:
			{
				AZ_CORE_ASSERT(false, "RendererAPI::None is not supported");
				return nullptr;
			}

			case RendererAPI::API::OpenGL:
			{
				return createScope<OpenGLRendererAPI>();
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}
}