/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_LAYOUT_TILE_H
#define GWEN_CONTROLS_LAYOUT_TILE_H

#include "Gwen/Controls/Base.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Layout
		{
			class GWEN_EXPORT Tile : public Base
			{
				public:

					GWEN_CONTROL_INLINE( Tile, Base )
					{
						Dock( Pos::Fill );
						SetTileSize( 22, 22 );
					}

					void PostLayout( Skin::Base* skin )
					{
						Gwen::Rect	bounds = GetInnerBounds();
						Gwen::Point pos = Point( bounds.x, bounds.y );

						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							Base* pChild = *it;

							if ( pChild->GetDock() != Pos::None ) { continue; }

							pChild->SetPos( pos.x + ( m_TileSize.x / 2 ) - ( pChild->Width() / 2 ), pos.y + ( m_TileSize.y / 2 ) - ( pChild->Height() / 2 ) );
							pos.x = pos.x + m_TileSize.x;

							if ( pos.x + m_TileSize.x > bounds.x + bounds.w )
							{
								pos.x = bounds.x;
								pos.y += m_TileSize.y;
							}
						}
					}

					void SetTileSize( int x, int y )
					{
						m_TileSize = Point( x, y );
					}

				private:

					Point	m_TileSize;
			};

		}
	}
}

#endif
