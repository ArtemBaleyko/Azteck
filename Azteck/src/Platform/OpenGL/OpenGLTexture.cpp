#include "azpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

namespace Azteck
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: _path(path)
		, _internalFormat(0)
		, _dataFormat(0)
	{
		AZ_PROFILE_FUNCTION();

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		AZ_CORE_ASSERT(data, "Failed to load image!");

		_width = static_cast<uint32_t>(width);
		_height = static_cast<uint32_t>(height);

		if (channels == 4)
		{
			_internalFormat = GL_RGBA8;
			_dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			_internalFormat = GL_RGB8;
			_dataFormat = GL_RGB;
		}

		AZ_CORE_ASSERT(_internalFormat & _dataFormat, "Image format is not supported");

		glCreateTextures(GL_TEXTURE_2D, 1, &_renderedId);
		glTextureStorage2D(_renderedId, 1, _internalFormat, _width, _height);

		glTextureParameteri(_renderedId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_renderedId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(_renderedId, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: _width(width)
		, _height(height)
		, _internalFormat(GL_RGBA8)
		, _dataFormat(GL_RGBA)
	{
		AZ_PROFILE_FUNCTION();

		glCreateTextures(GL_TEXTURE_2D, 1, &_renderedId);
		glTextureStorage2D(_renderedId, 1, _internalFormat, _width, _height);

		glTextureParameteri(_renderedId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_renderedId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		AZ_PROFILE_FUNCTION();

		glDeleteTextures(1, &_renderedId);
	}

	void OpenGLTexture2D::setData(void* data, uint32_t size)
	{
		AZ_PROFILE_FUNCTION();

		uint32_t bytesPerPixel = _dataFormat == GL_RGBA ? 4 : 3;
		AZ_CORE_ASSERT(size == _width * _height * bytesPerPixel, "Data must be entire texture");
		glTextureSubImage2D(_renderedId, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		AZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, _renderedId);
	}
}