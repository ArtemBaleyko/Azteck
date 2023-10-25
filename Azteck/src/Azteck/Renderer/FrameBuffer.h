#pragma once

#include "Azteck/Core/Core.h"

namespace Azteck
{
	struct FrameBufferSpecification
	{
		uint32_t width;
		uint32_t height;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual const FrameBufferSpecification& getSpecification() const = 0;
		virtual uint32_t getColorAttachmentRendererId() const = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecification& spec);
	};
}