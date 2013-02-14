/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_WINDOWCONTROL_H
#define GWEN_CONTROLS_WINDOWCONTROL_H

#include "Gwen/Gwen.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Dragger.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/ResizableControl.h"
#include "Gwen/Controls/Modal.h"
#include "Gwen/Controls/WindowButtons.h"
#include "Gwen/Skin.h"


namespace Gwen
{
	namespace Controls
	{
		class CloseButton;

		class GWEN_EXPORT WindowControl : public ResizableControl
		{
			public:

				GWEN_CONTROL( WindowControl, ResizableControl );

				virtual ~WindowControl();
				virtual void Render( Skin::Base* skin );
				virtual void RenderUnder( Skin::Base* skin );

				virtual void SetTitle( Gwen::UnicodeString title );
				virtual void SetTitle( Gwen::String title ) { SetTitle( Gwen::Utility::StringToUnicode( title ) ); }
				virtual void SetClosable( bool closeable );

				virtual void Touch();
				bool IsOnTop();

				virtual void SetHidden( bool hidden );

				void CloseButtonPressed();
				void RenderFocus( Gwen::Skin::Base* skin );
				void SetDeleteOnClose( bool b ) { m_bDeleteOnClose = b; }

				void MakeModal( bool bDrawBackground = true );
				void DestroyModal();

				Gwen::Event::Caller	onWindowClosed;

			protected:

				Gwen::ControlsInternal::Dragger*	m_TitleBar;
				Gwen::Controls::Label*				m_Title;
				Gwen::Controls::WindowCloseButton*	m_CloseButton;

				bool m_bDeleteOnClose;

				ControlsInternal::Modal* m_Modal;
		};
	}
}
#endif
