function _project(name)
	local settings = dofile "settings.lua"

	project(name)

	cppdialect("C++11")
	exceptionhandling("On")
	system(settings.system)
	warnings("Extra")

	location("../SubModules/")
	objdir("../%{cfg.location}/build/%{cfg.platform}/%{cfg.buildcfg}/")
	targetdir("../%{cfg.location}/bin/%{cfg.platform}/%{cfg.buildcfg}/")

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}
	flags {
		"MultiProcessorCompile"
	}

	-- Debug
	filter { "configurations:Debug" }
		optimize("Off")
		symbols("On")
	
	-- Release
	filter { "configurations:Release" }
		optimize("Full")
		symbols("Off")

	-- Profile
	filter { "configurations:Profile" }
		optimize("Full")
		symbols("On")
		
	-- GNU Make
	filter { "action:gmake" }
		cppdialect("gnu++11")

	-- not Windows
	filter { "system:not windows" }
		architecture(settings.architecture)

	filter {}
end