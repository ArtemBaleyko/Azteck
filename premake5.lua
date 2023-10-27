include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Azteck"
	architecture "x86_64"
	startproject "Azteck-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Azteck/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Azteck/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Azteck/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Azteck/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Azteck/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Azteck/vendor/entt/include"

group "Dependencies"
	include "vendor/premake"
	include "Azteck/vendor/GLFW"
	include "Azteck/vendor/Glad"
	include "Azteck/vendor/imgui"
group ""

include "Azteck"
include "Azteck-Editor"
include "Sandbox"
