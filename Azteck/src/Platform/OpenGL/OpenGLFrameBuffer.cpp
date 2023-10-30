#include "azpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Azteck
{
	// TODO: Use max value from GPU
	static constexpr uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	namespace Utils
	{
		static bool isDepthFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
				{
					return true;
				}
				default:
					return false;
			}
		}

		static GLenum textureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void createTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(textureTarget(multisampled), count, outID);
		}

		static void bindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(textureTarget(multisampled), id);
		}

		static void attachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTarget(multisampled), id, 0);
		}

		static void attachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget(multisampled), id, 0);
		}

		static GLenum toGLTextureFormat(FrameBufferTextureFormat format)
		{
			switch(format)
			{
				case FrameBufferTextureFormat::RGBA8: return GL_RGBA8;
				case FrameBufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			AZ_CORE_ASSERT(false, "Unsupported texture format");
			return 0;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: _rendererId(0)
		, _depthAttachment(0)
		, _spec(spec)
		, _depthAttachmentSpec(FrameBufferTextureFormat::None)
	{
		for (auto format : _spec.attachments.attachments)
		{
			if (!Utils::isDepthFormat(format.textureFormat))
				_colorAttachmentSpecs.emplace_back(format);
			else
				_depthAttachmentSpec = format;
		}

		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		cleanup();
	}

	void OpenGLFrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _rendererId);
		glViewport(0, 0, _spec.width, _spec.height);
	}

	void OpenGLFrameBuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			AZ_CORE_WARN("Invalid params for framebuffer resizing: ({0}, {1})", width, height);
			return;
		}

		_spec.width = width;
		_spec.height = height;

		invalidate();
	}

	int OpenGLFrameBuffer::readPixel(uint32_t attachmentIndex, int x, int y)
	{
		AZ_CORE_ASSERT(attachmentIndex < _colorAttachmentSpecs.size(), "Invalid attachment index");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	void OpenGLFrameBuffer::clearAttachment(uint32_t attachmentIndex, int value)
	{
		AZ_CORE_ASSERT(attachmentIndex < _colorAttachmentSpecs.size(), "Invalid attachment index");

		const auto& spec = _colorAttachmentSpecs[attachmentIndex];
		glClearTexImage(_colorAttachments[attachmentIndex], 0, Utils::toGLTextureFormat(spec.textureFormat), GL_INT, &value);
	}

	void OpenGLFrameBuffer::invalidate()
	{
		if (_rendererId)
			cleanup();

		glCreateFramebuffers(1, &_rendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, _rendererId);

		bool multisampled = _spec.samples > 1;

		// Attachments
		if (!_colorAttachmentSpecs.empty())
		{
			const size_t textureCount = _colorAttachmentSpecs.size();
			_colorAttachments.resize(textureCount);
			Utils::createTextures(multisampled, _colorAttachments.data(), textureCount);

			for (size_t i = 0; i < textureCount; i++)
			{
				Utils::bindTexture(multisampled, _colorAttachments[i]);

				switch (_colorAttachmentSpecs[i].textureFormat)
				{
					case FrameBufferTextureFormat::RGBA8:
					{
						Utils::attachColorTexture(_colorAttachments[i], _spec.samples, GL_RGBA8, GL_RGBA, _spec.width, _spec.height, i);
						break;
					}
					case FrameBufferTextureFormat::RED_INTEGER:
					{
						Utils::attachColorTexture(_colorAttachments[i], _spec.samples, GL_R32I, GL_RED_INTEGER, _spec.width, _spec.height, i);
						break;
					}
					default:
						break;
				}
			}
		}

		if (_depthAttachmentSpec.textureFormat != FrameBufferTextureFormat::None)
		{
			Utils::createTextures(multisampled, &_depthAttachment, 1);
			Utils::bindTexture(multisampled, _depthAttachment);

			switch (_depthAttachmentSpec.textureFormat)
			{
			case FrameBufferTextureFormat::DEPTH24STENCIL8:
			{
				Utils::attachDepthTexture(_depthAttachment, _spec.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, _spec.width, _spec.height);
				break;
			}

			default:
				break;
			}

		}

		const size_t colorAttachmentsCount = _colorAttachments.size();
		if (colorAttachmentsCount > 1)
		{
			AZ_CORE_ASSERT(colorAttachmentsCount <= 4, "Azteck supports up to 4 color attachments!");
			constexpr GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(colorAttachmentsCount, buffers);
		}
		else if (colorAttachmentsCount == 0)
		{
			glDrawBuffer(GL_NONE);
		}

		AZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frame buffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::cleanup()
	{
		glDeleteFramebuffers(1, &_rendererId);
		glDeleteTextures(_colorAttachments.size(), _colorAttachments.data());
		glDeleteTextures(1, &_depthAttachment);

		_colorAttachments.clear();
		_depthAttachment = 0;
	}
}
