#include "azpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Azteck
{
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
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
			return createRef<OpenGLVertexBuffer>(size);
		}

		default:
		{
			AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
			return nullptr;
		}
		}
	}

	Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size)
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
				return createRef<OpenGLVertexBuffer>(vertices, size);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count)
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
				return createRef<OpenGLIndexBuffer>(indices, count);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	//-----------BufferLayout----------------------------------------------------
	/////////////////////////////////////////////////////////////////////////////
	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: _elements(elements)
	{
		calculateOffsetsAndStride();
	}

	BufferLayout::BufferLayout()
		: _elements()
		, _stride(0)
	{
	}

	void BufferLayout::calculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		_stride = 0;

		for (auto& element : _elements)
		{
			element.offset = offset;
			offset += element.size;
			_stride += element.size;
		}
	}
}