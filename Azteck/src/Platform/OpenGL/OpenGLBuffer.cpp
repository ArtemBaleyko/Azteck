#include "azpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Azteck
{
	/////////////////////////////////////////////////////////////////////////////
	//-----------Vertex Buffer---------------------------------------------------
	/////////////////////////////////////////////////////////////////////////////
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		AZ_PROFILE_FUNCTION();

		glCreateBuffers(1, &_rendererId);
		glBindBuffer(GL_ARRAY_BUFFER, _rendererId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		AZ_PROFILE_FUNCTION();

		glDeleteBuffers(1, &_rendererId);
	}

	void OpenGLVertexBuffer::bind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, _rendererId);
	}

	void OpenGLVertexBuffer::unbind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	const Azteck::BufferLayout& OpenGLVertexBuffer::getLayout() const
	{
		return _layout;
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
	{
		_layout = layout;
	}

	/////////////////////////////////////////////////////////////////////////////
	//-----------Index Buffer----------------------------------------------------
	/////////////////////////////////////////////////////////////////////////////
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: _count(count)
	{
		AZ_PROFILE_FUNCTION();

		glCreateBuffers(1, &_rendererId);
		glBindBuffer(GL_ARRAY_BUFFER, _rendererId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		AZ_PROFILE_FUNCTION();

		glDeleteBuffers(1, &_rendererId);
	}

	uint32_t OpenGLIndexBuffer::getCount() const
	{
		return _count;
	}

	void OpenGLIndexBuffer::bind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererId);
	}

	void OpenGLIndexBuffer::unbind() const
	{
		AZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}