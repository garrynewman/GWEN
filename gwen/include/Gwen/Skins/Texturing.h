#pragma once
#ifndef GWEN_SKINS_TEXTURING_H
#define GWEN_SKINS_TEXTURING_H

#include "Gwen/Gwen.h"
#include "Gwen/Texture.h"

namespace Gwen
{
	namespace Skin
	{
		namespace Texturing
		{
			struct Single
			{
				Single()
				{
					texture = NULL;
				}

				void Init( Texture* pTexture, float x, float y, float w, float h )
				{
					texture = pTexture;
					float texw = texture->width;
					float texh = texture->height;
					uv[0] = x / texw;
					uv[1] = y / texh;
					uv[2] = ( x + w ) / texw;
					uv[3] = ( y + h ) / texh;
					this->iWidth = w;
					this->iHeight = h;
				}

				void Draw( Gwen::Renderer::Base* render, Gwen::Rect r, const Gwen::Color & col = Gwen::Colors::White )
				{
					if ( !texture ) { return; }

					render->SetDrawColor( col );
					render->DrawTexturedRect( texture, r, uv[0], uv[1], uv[2], uv[3] );
				}

				void DrawCenter( Gwen::Renderer::Base* render, Gwen::Rect r, const Gwen::Color & col = Gwen::Colors::White )
				{
					if ( !texture ) { return; }

					r.x += ( r.w - iWidth ) * 0.5;
					r.y += ( r.h - iHeight ) * 0.5;
					r.w = iWidth;
					r.h = iHeight;
					Draw( render, r, col );
				}


				Texture*	texture;
				float		uv[4];
				int			iWidth;
				int			iHeight;
			};

			struct Bordered
			{
				Bordered()
				{
					texture = NULL;
				}

				void Init( Texture* pTexture, float x, float y, float w, float h, Margin in_margin, float DrawMarginScale = 1.0 )
				{
					texture = pTexture;
					margin = in_margin;
					SetRect( 0, x, y, margin.left, margin.top );
					SetRect( 1, x + margin.left, y, w - margin.left - margin.right, margin.top );
					SetRect( 2, ( x + w ) - margin.right, y, margin.right, margin.top );
					SetRect( 3, x, y + margin.top, margin.left, h - margin.top - margin.bottom );
					SetRect( 4, x + margin.left,  y + margin.top, w - margin.left - margin.right, h - margin.top - margin.bottom );
					SetRect( 5, ( x + w ) - margin.right,  y + margin.top, margin.right, h - margin.top - margin.bottom );
					SetRect( 6, x, ( y + h ) - margin.bottom, margin.left, margin.bottom );
					SetRect( 7, x + margin.left, ( y + h ) - margin.bottom, w - margin.left - margin.right, margin.bottom );
					SetRect( 8, ( x + w ) - margin.right, ( y + h ) - margin.bottom, margin.right, margin.bottom );
					margin.left *= DrawMarginScale;
					margin.right *= DrawMarginScale;
					margin.top *= DrawMarginScale;
					margin.bottom *= DrawMarginScale;
					width = w - x;
					height = h - y;
				}

				void SetRect( int iNum, float x, float y, float w, float h )
				{
					float texw = texture->width;
					float texh = texture->height;
					//x -= 1.0f;
					//y -= 1.0f;
					rects[iNum].uv[0] = x / texw;
					rects[iNum].uv[1] = y / texh;
					rects[iNum].uv[2] = ( x + w ) / texw;
					rects[iNum].uv[3] = ( y + h ) / texh;
					//	rects[iNum].uv[0] += 1.0f / texture->width;
					//	rects[iNum].uv[1] += 1.0f / texture->width;
				}

				void Draw( Gwen::Renderer::Base* render, Gwen::Rect r, const Gwen::Color & col = Gwen::Colors::White, bool b1 = true, bool b2 = true, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true, bool b8 = true, bool b9 = true )
				{
					if ( !texture ) { return; }

					render->SetDrawColor( col );

					if ( r.w < width && r.h < height )
					{
						render->DrawTexturedRect( texture,
												  r,
												  rects[0].uv[0], rects[0].uv[1], rects[8].uv[2], rects[8].uv[3] );
						return;
					}

					if ( b1 ) { DrawRect( render, 0, r.x, r.y, margin.left, margin.top ); }

					if ( b2 ) { DrawRect( render, 1, r.x + margin.left, r.y, r.w - margin.left - margin.right, margin.top ); }

					if ( b3 ) { DrawRect( render, 2, ( r.x + r.w ) - margin.right, r.y, margin.right, margin.top ); }

					if ( b4 ) { DrawRect( render, 3, r.x, r.y + margin.top, margin.left, r.h - margin.top - margin.bottom ); }

					if ( b5 ) { DrawRect( render, 4, r.x + margin.left, r.y + margin.top, r.w - margin.left - margin.right, r.h - margin.top - margin.bottom ); }

					if ( b6 ) { DrawRect( render, 5, ( r.x + r.w ) - margin.right, r.y + margin.top, margin.right, r.h - margin.top - margin.bottom ); }

					if ( b7 ) { DrawRect( render, 6, r.x, ( r.y + r.h ) - margin.bottom, margin.left, margin.bottom ); }

					if ( b8 ) { DrawRect( render, 7, r.x + margin.left, ( r.y + r.h ) - margin.bottom, r.w - margin.left - margin.right, margin.bottom ); }

					if ( b9 ) { DrawRect( render, 8, ( r.x + r.w ) - margin.right, ( r.y + r.h ) - margin.bottom, margin.right, margin.bottom ); }
				}

				void DrawRect( Gwen::Renderer::Base* render, int i, int x, int y, int w, int h )
				{
					render->DrawTexturedRect( texture,
											  Gwen::Rect( x, y, w, h ),
											  rects[i].uv[0], rects[i].uv[1], rects[i].uv[2], rects[i].uv[3] );
				}

				Texture*	texture;

				struct SubRect
				{
					float uv[4];
				};

				SubRect rects[9];
				Margin margin;

				float width;
				float height;
			};
		}
	}
}
#endif
