#include "azpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Azteck
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Bool:		return GL_BOOL;
			default:
			{
				AZ_CORE_ASSERT(false, "Unknown ShaderDataType");
				return 0;
			}
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
		: _vertexBufferIndex(0)
	{
		AZ_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &_rendererId);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		AZ_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &_rendererId);
	}

	void OpenGLVertexArray::bind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindVertexArray(_rendererId);
	}

	void OpenGLVertexArray::unbind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		AZ_PROFILE_FUNCTION();

		glBindVertexArray(_rendererId);
		vertexBuffer->bind();

		AZ_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout");

		const BufferLayout& layout = vertexBuffer->getLayout();

		for (const BufferElement& element : layout)
		{
			switch (element.type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(_vertexBufferIndex);
					glVertexAttribPointer(_vertexBufferIndex,
						element.getComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE,
						layout.getStride(),
						(const void*)element.offset);
					_vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.getComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(_vertexBufferIndex);
						glVertexAttribPointer(_vertexBufferIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.type),
							element.normalized ? GL_TRUE : GL_FALSE,
							layout.getStride(),
							(const void*)(element.offset + sizeof(float) * count * i));
						glVertexAttribDivisor(_vertexBufferIndex, 1);
						_vertexBufferIndex++;
					}
					break;
				}
				default:
					AZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		AZ_PROFILE_FUNCTION();

		glBindVertexArray(_rendererId);
		indexBuffer->bind();

		_indexBuffer = indexBuffer;
	}

	const std::vector<Ref<Azteck::VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const
	{
		return _vertexBuffers;
	}

	const Ref<Azteck::IndexBuffer>& OpenGLVertexArray::getIndexBuffer() const
	{
		return _indexBuffer;
	}
}
