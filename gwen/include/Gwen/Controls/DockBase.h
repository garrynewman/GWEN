/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_DOCKBASE_H
#define GWEN_CONTROLS_DOCKBASE_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Button.h"

namespace Gwen
{
	namespace Controls
	{
		class DockedTabControl;
		class TabControl;

		class GWEN_EXPORT DockBase : public Base
		{
			public:

				GWEN_CONTROL( DockBase, Base );

				virtual void Render( Skin::Base* skin ) override;
				virtual void RenderOver( Skin::Base* skin ) override;
				virtual bool IsEmpty();

				virtual TabControl* GetTabControl();

				DockBase* GetRight() { return GetChildDock( Pos::Right ); }
				DockBase* GetLeft() { return GetChildDock( Pos::Left ); }
				DockBase* GetTop() { return GetChildDock( Pos::Top ); }
				DockBase* GetBottom() { return GetChildDock( Pos::Bottom ); }

				// No action on space (default button action is to press)
				virtual bool OnKeySpace( bool /*bDown*/ ) override { return false; }

			private:

				// Drag n Drop
				virtual bool DragAndDrop_HandleDrop( Gwen::DragAndDrop::Package* pPackage, int x, int y ) override;
				virtual bool DragAndDrop_CanAcceptPackage( Gwen::DragAndDrop::Package* pPackage ) override;
				virtual void DragAndDrop_HoverEnter( Gwen::DragAndDrop::Package* pPackage, int x, int y ) override;
				virtual void DragAndDrop_HoverLeave( Gwen::DragAndDrop::Package* pPackage ) override;
				virtual void DragAndDrop_Hover( Gwen::DragAndDrop::Package* pPackage, int x, int y ) override;

				virtual void SetupChildDock( int iPos );

				virtual void DoRedundancyCheck();
				virtual void DoConsolidateCheck();
				virtual void OnRedundantChildDock( DockBase* pDockBase );

				virtual int GetDroppedTabDirection( int x, int y );
				virtual void OnTabRemoved( Gwen::Controls::Base* pControl );

				DockBase* GetChildDock( int iPos );
				DockBase** GetChildDockPtr( int iPos );

				DockBase*		m_Left;
				DockBase*		m_Right;
				DockBase*		m_Top;
				DockBase*		m_Bottom;

				// Only CHILD dockpanels have a tabcontrol.
				DockedTabControl*		m_DockedTabControl;

				bool		m_bDrawHover;
				bool		m_bDropFar;
				Gwen::Rect		m_HoverRect;

		};
	}
}
#endif
