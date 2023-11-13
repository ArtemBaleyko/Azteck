#pragma once

#include <filesystem>

#include "Azteck/Core/Core.h"
#include "Azteck/Renderer/Texture.h"

namespace Azteck
{
	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		const MSDFData* getMSDFData() const { return _data; }
		Ref<Texture2D> getAtlasTexture() const { return _atlasTexture; }

		static Ref<Font> getDefault();

	private:
		MSDFData* _data;
		Ref<Texture2D> _atlasTexture;
	};

}
