project "Azteck-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs 
	{
		"%{wks.location}/Azteck/vendor/spdlog/include",
		"%{wks.location}/Azteck/src",
		"%{wks.location}/Azteck/vendor",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
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
