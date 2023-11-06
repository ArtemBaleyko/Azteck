#pragma once

#include "Azteck/Renderer/Texture.h"

namespace Azteck
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender();

	private:
		std::filesystem::path _currentDirectory;
		std::filesystem::path _baseDirectory;

		Ref<Texture2D> _directoryIcon;
		Ref<Texture2D> _fileIcon;
	};
}
