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
		stbi_uc* data = nullptr;
		{
			AZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			_isLoaded = true;

			_width = width;
			_height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			_internalFormat = internalFormat;
			_dataFormat = dataFormat;

			AZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &_renderedId);
			glTextureStorage2D(_renderedId, 1, internalFormat, _width, _height);

			glTextureParameteri(_renderedId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_renderedId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_renderedId, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(_renderedId, 0, 0, 0, _width, _height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
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
		glTextureParameteri(_renderedId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	bool OpenGLTexture2D::operator==(const Texture& other) const
	{
		if (const OpenGLTexture2D* openGLTexture = dynamic_cast<const OpenGLTexture2D*>(&other))
			return openGLTexture->_renderedId == _renderedId;

		return false;
	}
}
