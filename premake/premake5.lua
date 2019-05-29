require "_template"


local settings = dofile "settings.lua"

solution("SoftwareRenderer")
	startproject("SoftwareRenderer")

	location("..")

	configurations {
		"Debug",
		"Release",
	}

	-- Windows
	filter { "system:windows" }
		platforms {
			"x86",
			"x64",
		}

	-- not Windows
	filter { "system:not windows" }
		platforms {
			settings.architecture,
		}
		
	filter {}

_project("SoftwareRenderer")
	location("../SoftwareRenderer")
	
	debugdir("../")
	
	targetname("SoftwareRenderer")
	
	characterset("MBCS")	
	
	objdir("../SoftwareRenderer/build/%{cfg.platform}/%{cfg.buildcfg}/")
	targetdir("../SoftwareRenderer/bin/%{cfg.platform}/%{cfg.buildcfg}/")

	
	defines {
		os.host():upper(),
		"NOMINMAX",
		"USE_SIMD"
	}
	
	files {
		"../src/**.hpp",
		"../src/**.cpp",
		"../src/**.inl"
	}
	
	filter { "configurations:Debug" }
		kind("ConsoleApp")

	filter { "configurations:Release" }
		kind("WindowedApp")
		defines {
			"NDEBUG"
		}
	
	
	filter { "system:windows" }
		toolset("msc")
		links {
			"Shlwapi",
			"gdi32",
		}

	filter { "system:linux" }
		toolset("gcc")
		cppdialect "C++17"
		links {
			"X11"
		}
	
	filter {}

