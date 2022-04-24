#include "azpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Azteck
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Azteck::ShaderDataType::Int:		return GL_INT;
			case Azteck::ShaderDataType::Int2:		return GL_INT;
			case Azteck::ShaderDataType::Int3:		return GL_INT;
			case Azteck::ShaderDataType::Int4:		return GL_INT;
			case Azteck::ShaderDataType::Float:		return GL_FLOAT;
			case Azteck::ShaderDataType::Float2:	return GL_FLOAT;
			case Azteck::ShaderDataType::Float3:	return GL_FLOAT;
			case Azteck::ShaderDataType::Float4:	return GL_FLOAT;
			case Azteck::ShaderDataType::Mat3:		return GL_FLOAT;
			case Azteck::ShaderDataType::Mat4:		return GL_FLOAT;
			case Azteck::ShaderDataType::Bool:		return GL_BOOL;
			default:
			{
				AZ_CORE_ASSERT(false, "Unknown ShaderDataType");
				return 0;
			}
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &_rendererId);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &_rendererId);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(_rendererId);
	}

	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(_rendererId);
		vertexBuffer->bind();

		AZ_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout");

		uint32_t index = 0;
		const BufferLayout& layout = vertexBuffer->getLayout();

		for (const BufferElement& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(void*)element.offset);

			index++;
		}

		_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
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