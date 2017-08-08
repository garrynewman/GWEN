/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_OPENGL_DEBUGFONT_H
#define GWEN_RENDERERS_OPENGL_DEBUGFONT_H

#include "Gwen/Gwen.h"
#include "Gwen/Renderers/OpenGL.h"

namespace Gwen
{
	namespace Renderer
	{

		class OpenGL_DebugFont : public Gwen::Renderer::OpenGL
		{
			public:

				OpenGL_DebugFont();
				~OpenGL_DebugFont();

				void Init();

				void RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::UnicodeString & text );
				Gwen::PointF MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

			protected:

				void CreateDebugFont();
				void DestroyDebugFont();

				Gwen::Texture*	m_pFontTexture;
				float			m_fFontScale[2];
				float			m_fLetterSpacing;

		};

	}
}
#endif
