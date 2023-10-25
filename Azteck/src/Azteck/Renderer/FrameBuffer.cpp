#include "azpch.h"

#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer.h"

namespace Azteck
{
	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification& spec)
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
			return createRef<OpenGLFrameBuffer>(spec);
		}

		default:
		{
			AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
			return nullptr;
		}
		}
	}
}