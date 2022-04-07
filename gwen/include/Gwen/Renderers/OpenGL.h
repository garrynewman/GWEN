/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_OPENGL_H
#define GWEN_RENDERERS_OPENGL_H

#include "Gwen/Gwen.h"
#include "Gwen/Renderers/OpenGL_Base.h"

struct FONScontext;

namespace Gwen
{
	namespace Renderer
	{
		class OpenGL : public Gwen::Renderer::OpenGL_Base
		{
				// Fallback fonts for other languages/charsets
				int fallback_1 = -1;
				int fallback_2 = -1;

			public:
			
				virtual ~OpenGL();

				struct Vertex
				{
					float x, y, z;
					float u, v;
					unsigned char r, g, b, a;
				};

				virtual void Init();
				
				virtual void RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::TextObject & text )
				{
					RenderText( pFont, pos, text.Get() );
				}
		
				virtual Gwen::PointF MeasureText( Gwen::Font* pFont, const Gwen::TextObject & text )
				{
					return MeasureText( pFont, text.Get() );
				}
				
				virtual void RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::UnicodeString & text )
				{
					RenderText( pFont, pos, Gwen::Utility::UnicodeToString(text) );
				}
				
				virtual Gwen::PointF MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text )
				{
					return MeasureText( pFont, Gwen::Utility::UnicodeToString(text) );
				}

				void RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::String & text );
				Gwen::PointF MeasureText( Gwen::Font* pFont, const Gwen::String & text );
		};

	}
}
#endif
