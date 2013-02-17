/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_WINDOWS_BUTTONS_H
#define GWEN_CONTROLS_WINDOWS_BUTTONS_H

#include "Gwen/Gwen.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Skin.h"


namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT WindowCloseButton : public Button
		{
				GWEN_CONTROL_INLINE( WindowCloseButton, Button )
				{
					m_pWindow = NULL;
					SetSize( 31, 31 );
					SetText( "" );
				}

				virtual void Render( Skin::Base* skin )
				{
					if ( !m_pWindow ) { return; }

					skin->DrawWindowCloseButton( this, IsDepressed() && IsHovered(), IsHovered() && ShouldDrawHover(), IsDisabled() );
				}

				void SetWindow( Gwen::Controls::Base* p )
				{
					m_pWindow = p;
				}

			protected:

				Controls::Base* m_pWindow;
		};

		class GWEN_EXPORT WindowMaximizeButton : public WindowCloseButton
		{
				GWEN_CONTROL_INLINE( WindowMaximizeButton, WindowCloseButton ) { m_bMaximized = false; };

				virtual void Render( Skin::Base* skin )
				{
					if ( !m_pWindow ) { return; }

					skin->DrawWindowMaximizeButton( this, IsDepressed() && IsHovered(), IsHovered() && ShouldDrawHover(), IsDisabled(), m_bMaximized );
				}

				virtual void SetMaximized( bool b )
				{
					m_bMaximized = b;
				}

			protected:

				bool m_bMaximized;
		};

		class GWEN_EXPORT WindowMinimizeButton : public WindowCloseButton
		{
				GWEN_CONTROL_INLINE( WindowMinimizeButton, WindowCloseButton ) {};

				virtual void Render( Skin::Base* skin )
				{
					if ( !m_pWindow ) { return; }

					skin->DrawWindowMinimizeButton( this, IsDepressed() && IsHovered(), IsHovered() && ShouldDrawHover(), IsDisabled() );
				}

		};
	}
}

#endif
