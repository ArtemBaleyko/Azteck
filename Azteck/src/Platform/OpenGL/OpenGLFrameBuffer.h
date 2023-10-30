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
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

		virtual const FrameBufferSpecification& getSpecification() const override { return _spec; };
		virtual uint32_t getColorAttachmentRendererId(uint32_t index = 0) const override { 
			AZ_CORE_ASSERT(index < _colorAttachments.size(), "Invalid color attachment index");
			return _colorAttachments[index]; 
		};

		void invalidate();

	private:
		void cleanup();

	private:
		uint32_t _rendererId;
		FrameBufferSpecification _spec;

		uint32_t _depthAttachment;
		FramebufferTextureSpecification _depthAttachmentSpec;

		std::vector<uint32_t> _colorAttachments;
		std::vector<FramebufferTextureSpecification> _colorAttachmentSpecs;
	};
}
