#pragma once

#include "Azteck/Core/Buffers.h"

namespace Azteck {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static RawBuffer readFileBinary(const std::filesystem::path& filepath);
	};

}
