#pragma once

#include <string>
#include <optional>

namespace Azteck
{
	class FileDialogs
	{
	public:
		static std::optional<std::string> openFile(const char* filter);
		static std::optional<std::string> saveFile(const char* filter);
	};
}
