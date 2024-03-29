include "./vendor/premake/premake_customization/solution_items.lua"
include "dependencies.lua"

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

group "Dependencies"
	include "vendor/premake"
	include "Azteck/vendor/GLFW"
	include "Azteck/vendor/Glad"
	include "Azteck/vendor/imgui"
	include "Azteck/vendor/yaml-cpp"
	include "Azteck/vendor/Box2D"
	include "Azteck/vendor/msdf-atlas-gen"
group ""

group "Core"
	include "Azteck"
	include "Azteck-ScriptCore"
group ""

group "Tools"
	include "Azteck-Editor"
group ""

group "Misc"
	include "Sandbox"
group ""
