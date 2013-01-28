dofile( "inc/include.lua" )

solution "GWEN"

	language "C++"
	location ( os.get() .. "/" .. _ACTION )
	flags { "Unicode", "Symbols", "NoEditAndContinue", "NoPCH",
            "No64BitChecks", "StaticRuntime", "EnableSSE" } -- "NoRTTI"
	targetdir ( "../lib/" .. os.get() .. "/" .. _ACTION )
	libdirs { "../lib/", "../lib/" .. os.get() }

	configurations
	{
		"Release",
		"Debug"
	}

	if ( _ACTION == "vs2010" or _ACTION=="vs2008" ) then
		buildoptions { "/MP"  }
	end



configuration "Release"
	defines { "NDEBUG" }
	flags{ "Optimize", "FloatFast" }
	includedirs { "../include/" }

configuration "Debug"
	defines { "_DEBUG" }
	includedirs { "../include/" }
	targetsuffix( "d" )

project "GWEN-DLL"
	defines { "GWEN_COMPILE_DLL" }
	files { "../src/**.*", "../include/Gwen/**.*" }
	kind "SharedLib"
	targetname( "gwen" )

project "GWEN-Static"
	defines { "GWEN_COMPILE_STATIC" }
	files { "../src/**.*", "../include/Gwen/**.*" }
	flags { "Symbols" }
	kind "StaticLib"
	targetname( "gwen_static" )

project "UnitTest"
	files { "../UnitTest/**.*" }
	flags { "Symbols" }
	kind "StaticLib"
	targetname( "unittest" )

--
-- Renderers
--

DefineRenderer( "OpenGL",
                {"../Renderers/OpenGL/OpenGL.cpp"} )

DefineRenderer( "OpenGL_DebugFont",
                { "../Renderers/OpenGL/OpenGL.cpp",
                  "../Renderers/OpenGL/DebugFont/OpenGL_DebugFont.cpp" } )

DefineRenderer( "SFML",
                { "../Renderers/SFML/SFML.cpp" } )

DefineRenderer( "Allegro",
                { "../Renderers/Allegro/Allegro.cpp" } )

if ( os.get() == "windows" ) then
	DefineRenderer( "DirectX9",
                    { "../Renderers/DirectX9/DirectX9.cpp" } )

	DefineRenderer( "Direct2D",
                    { "../Renderers/Direct2D/Direct2D.cpp" } )

	DefineRenderer( "GDI",
                    { "../Renderers/GDIPlus/GDIPlus.cpp",
                      "../Renderers/GDIPlus/GDIPlusBuffered.cpp" } )
end

--
-- Samples
--

DefineSample( "CrossPlatform",
              { "../Samples/CrossPlatform/CrossPlatform.cpp" },
              { "UnitTest", "Renderer-OpenGL_DebugFont", "GWEN-Static", "FreeImage", "opengl32" },
              nil,
              { "USE_DEBUG_FONT" } )

DefineSample( "SFML",
              { "../Samples/SFML/SFML.cpp" },
              SFML_LIBS, SFML_LIBS_D )

DefineSample( "Allegro",
              { "../Samples/Allegro/AllegroSample.cpp" },
              ALLEGRO_LIBS, ALLEGRO_LIBS_D )

if ( os.get() == "windows" ) then

	DefineSample( "Direct2D",
                  { "../Samples/Direct2D/Direct2DSample.cpp" },
                  { "UnitTest", "Renderer-Direct2D", "GWEN-Static", "d2d1",
                    "dwrite", "windowscodecs" } )

	DefineSample( "DirectX9",
                  { "../Samples/Direct3D/Direct3DSample.cpp" },
                  { "UnitTest", "Renderer-DirectX9", "GWEN-Static" } )

	DefineSample( "WindowsGDI",
                  { "../Samples/WindowsGDI/WindowsGDI.cpp" },
                  { "UnitTest", "Renderer-GDI", "GWEN-Static" } )

	DefineSample( "OpenGL",
                  { "../Samples/OpenGL/OpenGLSample.cpp" },
                  { "UnitTest", "Renderer-OpenGL", "GWEN-Static", "FreeImage", "opengl32" } )

	DefineSample( "OpenGL_DebugFont",
                  { "../Samples/OpenGL/OpenGLSample.cpp" },
                  { "UnitTest", "Renderer-OpenGL_DebugFont", "GWEN-Static", "FreeImage", "opengl32" },
                  nil,
                  { "USE_DEBUG_FONT" } )

end

project "ControlFactory"
	files { "../Util/ControlFactory/**.*" }
	kind "StaticLib"
	targetname( "controlfactory" )

project "ImportExport"
	files { "../Util/ImportExport/**.*" }
	kind "StaticLib"
	targetname( "importexport" )
