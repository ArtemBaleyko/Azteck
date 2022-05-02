#pragma once

#include "Azteck/Renderer/Texture.h"

namespace Azteck
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		inline uint32_t getWidth() const override { return _width; };
		inline uint32_t getHeight() const override { return _height; };

		void bind(uint32_t slot = 0) const override;

	private:
		std::string _path;
		uint32_t _width;
		uint32_t _height;
		uint32_t _renderedId;
	};
}