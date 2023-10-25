#pragma once

#include "Azteck/Renderer/FrameBuffer.h"

namespace Azteck
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual const FrameBufferSpecification& getSpecification() const override { return _spec; };
		virtual uint32_t getColorAttachmentRendererId() const override { return _colorAttachment; };

		void invalidate();

	private:
		void cleanup();

	private:
		uint32_t _rendererId;
		uint32_t _colorAttachment;
		uint32_t _depthAttachment;
		FrameBufferSpecification _spec;
	};
}