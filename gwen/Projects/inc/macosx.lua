

if MONOLITHIC then
    ALLEGRO_LIBS	= { "UnitTest", "Renderer-Allegro", "GWEN-Static", "alleg" }
    ALLEGRO_LIBS_D	= { "UnitTest", "Renderer-Allegro", "GWEN-Static", "alleg" }
else
    -- Separate static libraries
    function allib(name) return "allegro" .. name .. "-static" end

    ALLEGRO_LIBS	= { "UnitTest", "Renderer-Allegro", "GWEN-Static",
                        "Cocoa.framework", "IOKit.framework", "OpenGL.framework",
                        "OpenAL.framework", "AudioToolbox.framework",
                        "freetype", "bz2", "z",
                         allib(""), allib("_main"), allib("_ttf"), allib("_font"),
                         allib("_primitives"), allib("_image") }

    ALLEGRO_LIBS_D	= ALLEGRO_LIBS
end

SFML_LIBS		= { "UnitTest", "Renderer-SFML", "GWEN-Static", "sfml-graphics",
                    "sfml-window", "sfml-system", "GL" }
SFML_LIBS_D		= { "UnitTest", "Renderer-SFML", "GWEN-Static", "sfml-graphics",
                    "sfml-window", "sfml-system", "GL" }
SFML_DEFINES	= { }

SFML2_LIBS		= { "UnitTest", "Renderer-SFML2", "GWEN-Static", "sfml-graphics",
                    "sfml-window", "sfml-system", "GL" }
SFML2_LIBS_D	= { "UnitTest", "Renderer-SFML2", "GWEN-Static", "sfml-graphics",
                    "sfml-window", "sfml-system", "GL" }
SFML2_DEFINES	= { }

CROSS_LIBS 		= { "UnitTest", "Renderer-OpenGL_DebugFont", "GWEN-Static", "GL", "freeimage" }