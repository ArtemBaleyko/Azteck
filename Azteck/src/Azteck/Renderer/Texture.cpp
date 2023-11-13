#include "azpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Azteck
{

	Ref<Azteck::Texture2D> Texture2D::create(const std::string& path)
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
				return createRef<OpenGLTexture2D>(path);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}

	Ref<Azteck::Texture2D> Texture2D::create(const TextureSpecification& specification)
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
			return createRef<OpenGLTexture2D>(specification);
		}

		default:
		{
			AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
			return nullptr;
		}
		}
	}

}
