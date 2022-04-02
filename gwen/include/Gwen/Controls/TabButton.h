/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_TABBUTTON_H
#define GWEN_CONTROLS_TABBUTTON_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Button.h"

namespace Gwen
{
	namespace Controls
	{
		class TabControl;
		class DockedTabControl;
		class DockBase;
		
				
		struct TabReturnButtonData
		{
			DockBase* dock;// original root dock this was a part of
			Base* window;// our parent window that we popped into
		};

		class GWEN_EXPORT TabButton : public Button
		{
			public:

				GWEN_CONTROL( TabButton, Button );
				virtual void Render( Skin::Base* skin );
				virtual void Layout( Skin::Base* skin );

				void SetPage( Base* page ) { m_Page = page; }
				Base* GetPage() { return m_Page; }

				void SetTabControl( TabControl* ctrl );
				TabControl* GetTabControl() { return m_Control; }
				void SetClosable(bool y) { m_bClosable = true; }
				void SetPopoutable(bool y) { DragAndDrop_GetPackage(0, 0)->canpopout = y; }

				bool IsClosable() { return m_bClosable; }
				bool IsPopoutable() { return DragAndDrop_GetPackage(0, 0)->canpopout; }
				bool IsActive() { return m_Page && m_Page->Visible(); }
				
				DockedTabControl* PopOut(int x = -1, int y = -1, TabReturnButtonData* out_data = 0);

				virtual bool DragAndDrop_ShouldStartDrag();
				virtual void DragAndDrop_StartDragging( Gwen::DragAndDrop::Package* /*pPackage*/, int /*x*/, int /*y*/ ) { SetHidden( true ); }
				virtual void DragAndDrop_EndDragging( bool /*bSuccess*/, int /*x*/, int /*y*/ );

				virtual bool OnKeyLeft( bool bDown );
				virtual bool OnKeyRight( bool bDown );
				virtual bool OnKeyUp( bool bDown );
				virtual bool OnKeyDown( bool bDown );

				virtual void UpdateColours();

				virtual bool ShouldClip() { return false; }

			private:

				Base*		m_Page;
				TabControl*	m_Control;
				bool        m_bClosable;

		};

	}
}
#endif
