/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_LAYOUT_POSITION_H
#define GWEN_CONTROLS_LAYOUT_POSITION_H

#include "Gwen/Controls/Label.h"
#include "Gwen/Utility.h"


namespace Gwen
{
	namespace Controls
	{
		namespace Layout
		{
			class GWEN_EXPORT Position : public Base
			{
				public:

					GWEN_CONTROL_INLINE( Position, Base )
					{
						m_iPosition = 0;
						SetPosition( Pos::Left | Pos::Top );
					}

					void PostLayout( Skin::Base* skin )
					{
						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							Base* pChild = *it;

							if ( pChild->GetDock() != Pos::None ) { continue; }

							pChild->Position( m_iPosition );
						}
					}

					void SetPosition( int p )
					{
						if ( m_iPosition == p ) { return; }

						m_iPosition = p;
						Invalidate();
					}

				private:

					int		m_iPosition;
			};

			class GWEN_EXPORT Center : public Position
			{
					GWEN_CONTROL_INLINE( Center, Position )
					{
						SetPosition( Pos::Center );
					}
			};
		}
	}
}
#endif
