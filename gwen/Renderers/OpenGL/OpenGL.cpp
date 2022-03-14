
#include "Gwen/Renderers/OpenGL.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <math.h>

#include "GL/glew.h"

#include <stdio.h>					// malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h>					// memset
#define FONTSTASH_IMPLEMENTATION	// Expands implementation
#include "FontStash/fontstash.h"

//#include <GLFW/glfw3.h>				// Or any other GL header of your choice.
#define GLFONTSTASH_IMPLEMENTATION	// Expands implementation
#include "FontStash/glfontstash.h"

namespace Gwen
{
	namespace Renderer
	{
        void OpenGL::Init()
        {
            fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
        }
        
        OpenGL::~OpenGL()
        {
        	glfonsDelete(fs);
        }


		void OpenGL::RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::UnicodeString & text )
		{
			float fSize = pFont->size * Scale();

			if ( !text.length() )
			{ return; }
			
			Flush();

            if (pFont->data == 0)
            {
                Gwen::String name = Gwen::Utility::UnicodeToString( pFont->facename );
                int font = fonsAddFont(fs, name.c_str(), "OpenSans.ttf");
                if (font == FONS_INVALID)
                {
		            printf("Could not add font %s.\n", name.c_str());
		            return;
	            }
                pFont->data = (void*)font;
            }

			Gwen::String converted_string = Gwen::Utility::UnicodeToString( text );

			pos.x += m_RenderOffset.x;
			pos.y += m_RenderOffset.y;
			pos.x = (((float)pos.x) * m_fScale);
			pos.y = (((float)pos.y) * m_fScale);

            fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
            fonsSetFont(fs, (long)pFont->data);
            fonsSetColor(fs, glfonsRGBA(m_Color.r,m_Color.g,m_Color.b,m_Color.a));
            fonsSetSize(fs, fSize*1.333f);
            fonsDrawText(fs, pos.x, pos.y, converted_string.c_str(), NULL);
		}

		Gwen::PointF OpenGL::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text )
		{
            if (pFont->data == 0)
            {
                Gwen::String name = Gwen::Utility::UnicodeToString( pFont->facename );
                pFont->data = (void*)fonsAddFont(fs, name.c_str(), "OpenSans.ttf");
            }

			float fSize = pFont->size * Scale();
			Gwen::String converted_string = Gwen::Utility::UnicodeToString( text );

            fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
            
            fonsSetFont(fs, (long)pFont->data);
            fonsSetSize(fs, fSize*1.333f);

			Gwen::PointF p;
			
            float bounds[4];//0 is minx, 1 is miny, 2 is maxx 3 is maxy
            float advance = fonsTextBounds(fs, 0, 0, converted_string.c_str(), NULL, bounds);
            p.x = advance;//std::abs(bounds[2] - bounds[0]);
			p.y = fSize*1.33;//std::abs(bounds[3] - bounds[1]);
			return p;
		}
	}
}
