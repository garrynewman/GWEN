solution "Designer"

	language "C++"
	location ( os.get() .. "/" .. _ACTION )
	flags { "Unicode", "Symbols", "NoEditAndContinue", "NoPCH", "No64BitChecks", "StaticRuntime", "EnableSSE" }
	targetdir ( "../bin" )
	debugdir ( "../bin" )
	libdirs { "../../gwen/lib/", "../../gwen/lib/" .. os.get() .. "/" .. _ACTION }
	includedirs { "../../gwen/include/", "../src/" }
	configurations { "Release", "Debug" }
	
configuration "Release"
	defines { "NDEBUG" }
	flags{ "Optimize", "FloatFast" }
	
configuration "Debug"
	defines { "_DEBUG" }
	includedirs { "../include/" }

project "Designer"
	files { "../src/**.*" }
	kind "WindowedApp"
	links { "GWEN-Renderer-DirectX9", "gwen_static", "bootil_static", "controlfactory", "importexport" }
