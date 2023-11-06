#pragma once

#include "Project.h"

namespace Azteck
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project);

		bool serialize(const std::filesystem::path& filepath);
		bool deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Project> _project;
	};
}
