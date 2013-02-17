/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_DIRECTX9_H
#define GWEN_RENDERERS_DIRECTX9_H

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#include <D3D9.h>
#include <D3DX9Core.h>

#pragma comment( lib, "D3D9.lib" )
#pragma comment( lib, "D3Dx9.lib" )

namespace Gwen
{
	namespace Renderer
	{

		class GWEN_EXPORT DirectX9 : public Gwen::Renderer::Base
		{
			public:

				DirectX9( IDirect3DDevice9* pDevice = NULL );
				~DirectX9();

				virtual void Begin();
				virtual void End();
				virtual void Release();

				virtual void SetDrawColor( Gwen::Color color );

				virtual void DrawFilledRect( Gwen::Rect rect );

				virtual void LoadFont( Gwen::Font* pFont );
				virtual void FreeFont( Gwen::Font* pFont );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

				void StartClip();
				void EndClip();

				void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
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

				virtual void FillPresentParameters( Gwen::WindowProvider* pWindow, D3DPRESENT_PARAMETERS & Params );

			protected:

				void*				m_pCurrentTexture;
				IDirect3DDevice9*	m_pDevice;
				IDirect3D9*			m_pD3D;
				DWORD				m_Color;
				Gwen::Font::List	m_FontList;
				//Gwen::Texture::List	m_TextureList;

				void Flush();
				void AddVert( int x, int y );
				void AddVert( int x, int y, float u, float v );

			protected:

				struct VertexFormat
				{
					FLOAT x, y, z, rhw;
					DWORD color;
					float u, v;
				};

				static const int		MaxVerts = 1024;
				VertexFormat			m_pVerts[MaxVerts];
				int						m_iVertNum;

		};

	}
}
#endif
