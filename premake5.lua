workspace "Azteck"
	architecture "x86_64"
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
IncludeDir["glm"] = "Azteck/vendor/glm"
IncludeDir["stb_image"] = "Azteck/vendor/stb_image"

group "Dependencies"
	include "Azteck/vendor/GLFW"
	include "Azteck/vendor/Glad"
	include "Azteck/vendor/imgui"
group ""

project "Azteck"
	location "Azteck"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "azpch.h"
	pchsource "Azteck/src/azpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{

		}

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AZ_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"Azteck/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"Azteck"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AZ_DIST"
		runtime "Release"
		optimize "on"
