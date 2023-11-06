#include "azpch.h"
#include "ProjectSerializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Azteck
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: _project(project)
	{
	}

	bool ProjectSerializer::serialize(const std::filesystem::path& filepath)
	{
		const auto& config = _project->getConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.name;
				out << YAML::Key << "StartScene" << YAML::Value << config.startScenePath.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.assetDirectory.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.scriptModulePath.string();
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::deserialize(const std::filesystem::path& filepath)
	{
		auto& config = _project->getConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			AZ_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.name = projectNode["Name"].as<std::string>();
		config.startScenePath = projectNode["StartScene"].as<std::string>();
		config.assetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.scriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
}
