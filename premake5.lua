workspace "Azteck"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Azteck"
	location "Azteck"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"AZ_PLATFORM_WINDOWS",
			"AZ_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "AZ_DIST"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

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
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"AZ_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "AZ_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "AZ_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "AZ_DIST"
		optimize "On"
