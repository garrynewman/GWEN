/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_IMAGEPANEL_H
#define GWEN_CONTROLS_IMAGEPANEL_H

#include "Gwen/Gwen.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/BaseRender.h"
#include "Gwen/Texture.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT ImagePanel : public Controls::Base
		{
			public:

				GWEN_CONTROL_INLINE( ImagePanel, Controls::Base )
				{
					SetUV( 0, 0, 1, 1 );
					SetMouseInputEnabled( false );
					m_DrawColor = Colors::White;
					SetStretch( true );
				}

				virtual ~ImagePanel()
				{
					m_Texture.Release( GetSkin()->GetRender() );
				}

				virtual void SetUV( float u1, float v1, float u2, float v2 )
				{
					m_uv[0] = u1;
					m_uv[1] = v1;
					m_uv[2] = u2;
					m_uv[3] = v2;
				}

				virtual void SetImage( const TextObject & imageName )
				{
					m_Texture.Load( imageName, GetSkin()->GetRender() );
				}

				virtual TextObject & GetImage()
				{
					return m_Texture.name;
				}

				virtual int TextureWidth()
				{
					return m_Texture.width;
				}

				virtual int TextureHeight()
				{
					return m_Texture.height;
				}

				virtual const TextObject & GetImageName()
				{
					return m_Texture.name;
				}

				virtual void Render( Skin::Base* skin )
				{
					skin->GetRender()->SetDrawColor( m_DrawColor );

					if ( m_bStretch )
					{ skin->GetRender()->DrawTexturedRect( &m_Texture, GetRenderBounds(), m_uv[0], m_uv[1], m_uv[2], m_uv[3] ); }
					else
					{ skin->GetRender()->DrawTexturedRect( &m_Texture, Gwen::Rect( 0, 0, m_Texture.width, m_Texture.height ), m_uv[0], m_uv[1], m_uv[2], m_uv[3] ); }
				}

				virtual void SizeToContents()
				{
					SetSize( m_Texture.width, m_Texture.height );
				}

				virtual void SetDrawColor( Gwen::Color color )
				{
					m_DrawColor = color;
				}

				virtual bool FailedToLoad()
				{
					return m_Texture.FailedToLoad();
				}

				virtual bool GetStretch() { return m_bStretch; }
				virtual void SetStretch( bool b ) { m_bStretch = b; }

			protected:

				Texture			m_Texture;
				float			m_uv[4];
				Gwen::Color		m_DrawColor;

				bool			m_bStretch;

		};
	}
}
#endif
