#pragma once

#include "Azteck/Core/Core.h"

namespace Azteck
{
	struct ProjectConfig
	{
		std::string name = "Untitled";

		std::filesystem::path startScenePath;
		std::filesystem::path assetDirectory;
		std::filesystem::path scriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& getProjectDirectory();
		static std::filesystem::path getAssetDirectory();
		static std::filesystem::path getAssetFileSystemPath(const std::filesystem::path& path);

		ProjectConfig& getConfig() { return _config; }
		static Ref<Project> getActive() { return _activeProject; }

		static Ref<Project> create();
		static Ref<Project> load(const std::filesystem::path& path);
		static bool saveActive(const std::filesystem::path& path);

	private:
		ProjectConfig _config;
		std::filesystem::path _projectDirectory;

		inline static Ref<Project> _activeProject;
	};
}
