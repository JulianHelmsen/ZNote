workspace "ZNote"
	architecture ("x86_64")
	systemversion "latest"
	configurations {"DEBUG", "RELEASE"}
	platforms {"WINDOWS", "LINUX" }



project "ZNote"
	kind "ConsoleApp"
	location "ZNote"
	cppdialect "C++17"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files {
		"ZNote/src/**.cpp",
		"ZNote/src/**.h"
	}

	includedirs {
		"external libraries/glm",
		"external libraries/stb",
		"ZNote/src",
		"external libraries/GLEW/include",
		"external libraries/FreeType/include"
	}

	
	libdirs {
		"external libraries/GLEW/lib/Release/x64",
		"external libraries/GLFW/lib-vc2017"
	}

	links {
		"Opengl32",
		"glew32s",
		"freetype.lib"
	}
	defines {"GLEW_STATIC"}

	filter "platforms:WINDOWS"
		files {
			"ZNote/platform/windows/**.cpp",
			"ZNote/platform/windows/**.h"
		}
		defines {"WINDOWS"}

	filter "platforms:LINUX"
		files {
			"ZNote/platform/linux/**.cpp",
			"ZNote/platform/linux/**.h"
		}
		defines {"LINUX"}


		links {
			"glfw3.lib"
		}

		includedirs {
			"external libraries/GLFW/include"
		}
	



	filter "configurations:DEBUG"
		defines {"DEBUG"}
		symbols "On"
		runtime "Debug"
		libdirs {
			"external libraries/FreeType/lib/DEBUG/"
		}
	
	filter "configurations:RELEASE"
		defines {"RELEASE", "NDEBUG"}
		optimize "Speed"
		runtime "Release"
		floatingpoint "fast"
		libdirs {
			"external libraries/FreeType/lib/RELEASE/"
		}
		

	
