workspace "Azteck"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Azteck/vendor/GLFW/include"
IncludeDir["Glad"] = "Azteck/vendor/Glad/include"
IncludeDir["ImGui"] = "Azteck/vendor/imgui"

group "Dependencies"
	include "Azteck/vendor/GLFW"
	include "Azteck/vendor/Glad"
	include "Azteck/vendor/imgui"
group ""

project "Azteck"
	location "Azteck"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "azpch.h"
	pchsource "Azteck/src/azpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"AZ_PLATFORM_WINDOWS",
			"AZ_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "AZ_DIST"
		runtime "Release"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"Azteck/vendor/spdlog/include",
		"Azteck/src"
	}

	links
	{
		"Azteck"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"AZ_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "AZ_DIST"
		runtime "Release"
		optimize "On"
