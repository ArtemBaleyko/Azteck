#pragma once

#include "Azteck/Core/Core.h"

namespace Azteck
{
	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Colors
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification(FrameBufferTextureFormat format)
			: textureFormat(format)
		{}

		FrameBufferTextureFormat textureFormat = FrameBufferTextureFormat::None;
		// TODO: Filtering, Wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: attachments(attachments)
		{}

		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t width = 0;
		uint32_t height = 0;

		FramebufferAttachmentSpecification attachments;

		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FrameBufferSpecification& getSpecification() const = 0;
		virtual uint32_t getColorAttachmentRendererId(uint32_t index = 0) const = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecification& spec);
	};
}
