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
            	FONScontext* fs = 0;
            
			public:
			
				virtual ~OpenGL();

				struct Vertex
				{
					float x, y, z;
					float u, v;
					unsigned char r, g, b, a;
				};

				virtual void Init();

				void RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::UnicodeString & text );
				Gwen::PointF MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );
		};

	}
}
#endif
