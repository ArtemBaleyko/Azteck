#pragma once

#include "Azteck/Renderer/VertexArray.h"

namespace Azteck
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const override;
		virtual const Ref<IndexBuffer>& getIndexBuffer() const override;

	private:
		std::vector<Ref<VertexBuffer>> _vertexBuffers;
		Ref<IndexBuffer> _indexBuffer;
		uint32_t _rendererId;
	};
}