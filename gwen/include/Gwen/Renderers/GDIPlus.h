/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_GDIPLUS_H
#define GWEN_RENDERERS_GDIPLUS_H

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#include <gdiplus.h>

/*

 GDI(plus) is pretty slow for rendering GWEN, because we're
 re-rendering everything on redraw.

 Therefore its usage should be as a test - rather than production.

*/

namespace Gwen
{
	namespace Renderer
	{

		class GDIPlus : public Gwen::Renderer::Base
		{
			public:

				GDIPlus( HWND pHWND = NULL );
				~GDIPlus();

				virtual void Begin();
				virtual void End();

				virtual void SetDrawColor( Gwen::Color color );

				virtual void DrawFilledRect( Gwen::Rect rect );

				virtual void LoadFont( Gwen::Font* pFont );
				virtual void FreeFont( Gwen::Font* pFont );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

				void StartClip();
				void EndClip();

				void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f );
				void LoadTexture( Gwen::Texture* pTexture );
				void FreeTexture( Gwen::Texture* pTexture );
				Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default );

			public:

				//
				// Self Initialization
				//

				virtual bool InitializeContext( Gwen::WindowProvider* pWindow );
				virtual bool ShutdownContext( Gwen::WindowProvider* pWindow );
				virtual bool PresentContext( Gwen::WindowProvider* pWindow );
				virtual bool ResizedContext( Gwen::WindowProvider* pWindow, int w, int h );
				virtual bool BeginContext( Gwen::WindowProvider* pWindow );
				virtual bool EndContext( Gwen::WindowProvider* pWindow );

			protected:

				int m_iWidth;
				int m_iHeight;

				Gdiplus::Color	m_Colour;

				HWND			m_HWND;
				HDC				m_hDC;
				ULONG_PTR       m_gdiplusToken;

				Gdiplus::Graphics*		graphics;
		};

		class GDIPlusBuffered : public GDIPlus
		{
			public:

				GDIPlusBuffered( HWND pHWND = NULL );
				~GDIPlusBuffered();

				virtual void Begin();
				virtual void End();

			private:

				void CreateBackbuffer();
				void DestroyBackbuffer();

				Gdiplus::Bitmap*			m_Bitmap;

		};
	}
}
#endif
