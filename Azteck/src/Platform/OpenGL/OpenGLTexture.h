#pragma once

#include "Azteck/Renderer/Texture.h"

#include <glad/glad.h>

namespace Azteck
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(const TextureSpecification& specification);

		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& getSpecification() const override { return _spec; }

		void setData(void* data, uint32_t size) override;

		inline uint32_t getWidth() const override { return _width; };
		inline uint32_t getHeight() const override { return _height; };
		inline uint32_t getRendererID() const override { return _renderedId; };
		inline const std::string& getPath() const override { return _path; };

		virtual bool isLoaded() const override { return _isLoaded; }

		void bind(uint32_t slot = 0) const override;

		bool operator==(const Texture& other) const override;

	private:
		TextureSpecification _spec;

		std::string _path;
		uint32_t _width;
		uint32_t _height;
		uint32_t _renderedId;

		GLenum _internalFormat;
		GLenum _dataFormat;

		bool _isLoaded;
	};
}
