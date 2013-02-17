/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_DIRECT2D_H
#define GWEN_RENDERERS_DIRECT2D_H

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"
#include "Gwen/Texture.h"

namespace Gwen
{
	namespace Renderer
	{

		class GWEN_EXPORT Direct2D : public Gwen::Renderer::Base
		{
			public:

				Direct2D();
				Direct2D( ID2D1RenderTarget* pDevice, IDWriteFactory* pDWriteFactory, IWICImagingFactory* pWICFactory );
				~Direct2D();

				virtual void Begin();
				virtual void End();
				virtual void Release();

				virtual void SetDrawColor( Gwen::Color color );

				virtual void DrawFilledRect( Gwen::Rect rect );

				virtual void LoadFont( Gwen::Font* pFont );
				virtual void FreeFont( Gwen::Font* pFont );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

				virtual void DeviceLost();
				virtual void DeviceAcquired( ID2D1RenderTarget* pRT );

				void StartClip();
				void EndClip();

				void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
				void LoadTexture( Gwen::Texture* pTexture );
				void FreeTexture( Gwen::Texture* pTexture );
				Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default );

				void DrawLinedRect( Gwen::Rect rect );
				void DrawShavedCornerRect( Gwen::Rect rect, bool bSlight = false );

			public:

				//
				// Self Initialization
				//

				bool InitializeContext( Gwen::WindowProvider* pWindow );
				bool ShutdownContext( Gwen::WindowProvider* pWindow );
				bool PresentContext( Gwen::WindowProvider* pWindow );
				bool ResizedContext( Gwen::WindowProvider* pWindow, int w, int h );
				bool BeginContext( Gwen::WindowProvider* pWindow );
				bool EndContext( Gwen::WindowProvider* pWindow );

			private:
				bool InternalCreateDeviceResources();
				void InternalReleaseDeviceResources();

				ID2D1Factory*	m_pD2DFactory;
				HWND			m_pHWND;

			private:
				bool InternalLoadTexture( Gwen::Texture* pTexture );
				bool InternalLoadFont( Gwen::Font* pFont );

				void InternalFreeFont( Gwen::Font* pFont, bool bRemove = true );
				void InternalFreeTexture( Gwen::Texture* pTexture, bool bRemove = true );

			private:

				IDWriteFactory*		m_pDWriteFactory;
				IWICImagingFactory*	m_pWICFactory;
				ID2D1RenderTarget*	m_pRT;

				ID2D1SolidColorBrush* m_pSolidColorBrush;

				D2D1::ColorF		m_Color;

				Gwen::Texture::List	m_TextureList;
				Gwen::Font::List	m_FontList;

		};

	}
}
#endif
