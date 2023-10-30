#include "azpch.h"
#include "UniformBuffer.h"

#include "Azteck/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Azteck 
{
	Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:    AZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return createRef<OpenGLUniformBuffer>(size, binding);
		}

		AZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
