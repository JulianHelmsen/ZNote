workspace "Notes"
	architecture ("x86_64")
	systemversion "latest"
	configurations {"DEBUG", "RELEASE"}
	platforms {"WINDOWS"}



project "Notes"
	kind "ConsoleApp"
	location "Notes"
	cppdialect "C++17"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files {
		"Notes/src/**.cpp",
		"Notes/src/**.h"
	}

	includedirs {
		"external libraries/glm",
		"external libraries/stb",
		"Notes/src",
		"external libraries/GLFW/include",
		"external libraries/GLEW/include"
	}

	
	libdirs {
		"external libraries/GLEW/lib/Release/x64",
		"external libraries/GLFW/lib-vc2017"
	}

	links {
		"glfw3.lib",
		"Opengl32",
		"glew32s"
	}
	defines {"GLEW_STATIC"}

	filter "platforms:WINDOWS"
		files {"Notes/platform/windows/**.cpp"}
		defines {"WINDOWS"}

	filter "configurations:DEBUG"
		defines {"DEBUG"}
		symbols "On"
		runtime "Debug"
	
	filter "configurations:RELEASE"
		defines {"RELEASE", "NDEBUG"}
		optimize "On"
		runtime "Release"
	
