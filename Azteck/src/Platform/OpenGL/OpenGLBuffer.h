#pragma once

#include "Azteck/Renderer/Buffer.h"

namespace Azteck
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const BufferLayout& getLayout() const override;
		virtual void setLayout(const BufferLayout& layout) override;

		virtual void setData(const void* data, uint32_t size) override;

	private:
		uint32_t _rendererId;
		BufferLayout _layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* vertices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual uint32_t getCount() const override;

		virtual void bind() const override;
		virtual void unbind() const override;

	private:
		uint32_t _rendererId;
		uint32_t _count;
	};
}