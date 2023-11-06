#include "azpch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Azteck
{
	const std::filesystem::path& Project::getProjectDirectory()
	{
		AZ_CORE_ASSERT(_activeProject, "Active project is nullptr");
		return _activeProject->_projectDirectory;
	}

	std::filesystem::path Project::getAssetDirectory()
	{
		AZ_CORE_ASSERT(_activeProject, "Active project is nullptr");
		return getProjectDirectory() / _activeProject->_config.assetDirectory;
	}

	std::filesystem::path Project::getAssetFileSystemPath(const std::filesystem::path& path)
	{
		return getAssetDirectory() / path;
	}

	Ref<Project> Project::create()
	{
		_activeProject = createRef<Project>();
		return _activeProject;
	}

	Ref<Project> Project::load(const std::filesystem::path& path)
	{
		Ref<Project> project = createRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.deserialize(path))
		{
			project->_projectDirectory = path.parent_path();
			_activeProject = project;
			return _activeProject;
		}

		return nullptr;
	}

	bool Project::saveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(_activeProject);
		if (serializer.serialize(path))
		{
			_activeProject->_projectDirectory = path.parent_path();
			return true;
		}

		return false;
	}
}
