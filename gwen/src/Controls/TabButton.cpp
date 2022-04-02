/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/TabButton.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/Highlight.h"
#include "Gwen/DragAndDrop.h"
#include "Gwen/Application.h"
#include "Gwen/Controls/DockedTabControl.h"
#include "Gwen/Controls/WindowCanvas.h"
#include "Gwen/Controls/WindowControl.h"

using namespace Gwen;
using namespace Gwen::Controls;


GWEN_CONTROL_CONSTRUCTOR( TabButton )
{
	m_bClosable = false;
	m_CloseButton = NULL;
	m_Page = NULL;
	m_Control = NULL;
	DragAndDrop_SetPackage( true, "TabButtonMove" );
	SetAlignment( Pos::Top | Pos::Left );
	SetTextPadding( Padding( 2, 2, 2, 2 ) );
}

void TabButton::Layout( Skin::Base* skin )
{
	int iParentDock = m_Control->GetTabStrip()->GetDock();

	if ( iParentDock == Pos::Bottom )
	{ SetPadding( Padding( 3, 1, 5, 4 ) ); }
	else if ( iParentDock == Pos::Top )
	{ SetPadding( Padding( 3, 3, 5, 2 ) ); }
	else
	{ SetPadding( Padding( 3, 2, 5, 2 ) ); }

	BaseClass::Layout( skin );
}

void TabButton::Render( Skin::Base* skin )
{
	skin->DrawTabButton( this, IsActive(), m_Control->GetTabStrip()->GetDock() );
}

void TabButton::SetTabControl( TabControl* ctrl )
{
	if ( m_Control == ctrl ) { return; }

	if ( m_Control )
	{
		m_Control->OnLoseTab( this );
	}

	m_Control = ctrl;
}

void TabButton::SetClosable(bool y)
{
	if (y == m_bClosable) { return; }
	
	m_bClosable = y;
	
	if (y)
	{
		m_CloseButton = new Controls::Button(this);
		m_CloseButton->SetSize(20, 20);
		m_CloseButton->SetPos(0, 2);
		m_CloseButton->SetText("x");
		m_CloseButton->Dock(Pos::Right);
		m_CloseButton->onPress.Add(this, &ThisClass::OnCloseButton);
	}
	else
	{
		m_CloseButton->DelayedDelete();
		m_CloseButton = NULL;
	}
}

bool TabButton::DragAndDrop_ShouldStartDrag()
{
	return m_Control->DoesAllowDrag();
}

void TabButton::DragAndDrop_EndDragging( bool bSuccess, int x, int y )
{
	SetHidden( false );
	SetDepressed( false );
	
	if (!bSuccess && Gwen::DragAndDrop::CurrentPackage->canpopout )
	{
		// pop out into window
		Gwen::Point p = GetCanvas()->WindowPosition();
		PopOut(p.x + x - Gwen::DragAndDrop::CurrentPackage->holdoffset.x,
		       p.y + y - Gwen::DragAndDrop::CurrentPackage->holdoffset.y);
	}
}

DockedTabControl* TabButton::PopOut(int x, int y, TabReturnButtonData* out_data)
{
	// first, lets iterate up to find the topmost dockbase
	//okay, we need to find the dockbase we came from so we can unpin later
	//also preferrably find the side we were pinned to
				
	TabControl* tab = GetTabControl();
	// now go up until we find a DockBase
	Base* current = tab;
	DockBase* dock = dynamic_cast<DockBase*>(current);
	while (current && dock == 0)
	{
		current = current->GetParent();
		dock = dynamic_cast<DockBase*>(current);
	}
	if (dock)
	{
		// now go to the top dock base, this one is unlikely to be removed
		while (dynamic_cast<DockBase*>(dock->GetParent()) != 0)
		{
			dock = dynamic_cast<DockBase*>(dock->GetParent());
		}
	}
	else
	{
		return 0;
	}
	
	// pop out into window
	auto page = GetPage();
	Gwen::Controls::Base* win;
	if (gApplication)
	{
		Gwen::Point mpos = Gwen::Input::GetMousePosition();
		win = gApplication->AddWindow("", page->Width(), page->Height() + 40, x, y);
	}
	else
	{
		auto wc = new WindowControl( GetCanvas() );
		wc->SetBounds( x, y, page->Width(), page->Height() + 40 );
		wc->SetDeleteOnClose(true);
		win = wc;
	}
	
	// Create a new tab control in that window so we can properly move ourself into it
	auto dcontrol = new DockedTabControl(win);
	dcontrol->Dock(Pos::Fill);
	dcontrol->SetShowTitlebar(true);
	dcontrol->m_WindowControl = win;
	dcontrol->AddPage(this);
	
	TabReturnButtonData* data = new TabReturnButtonData;
	data->window = dcontrol->m_WindowControl;
	data->dock = dock;
	UserData.Set<TabReturnButtonData*>("return_data", data);
	
	if (out_data)
	{
		*out_data = *data;
	}
	
	Invalidate();
	
	return dcontrol;
}

bool TabButton::OnKeyUp( bool bDown )
{
	OnKeyLeft( bDown );
	return true;
}
bool TabButton::OnKeyDown( bool bDown )
{
	OnKeyRight( bDown );
	return true;
}

bool TabButton::OnKeyLeft( bool bDown )
{
	if ( bDown )
	{
		Base::List::reverse_iterator it = std::find( m_Parent->Children.rbegin(), m_Parent->Children.rend(), this );

		if ( it != m_Parent->Children.rend() && ( ++it != m_Parent->Children.rend() ) )
		{
			Base* pNextTab = *it;
			GetTabControl()->OnTabPressed( pNextTab );
			Gwen::KeyboardFocus = pNextTab;
		}
	}

	return true;
}

bool TabButton::OnKeyRight( bool bDown )
{
	if ( bDown )
	{
		Base::List::iterator it = std::find( m_Parent->Children.begin(), m_Parent->Children.end(), this );

		if ( it != m_Parent->Children.end() && ( ++it != m_Parent->Children.end() ) )
		{
			Base* pNextTab = *it;
			GetTabControl()->OnTabPressed( pNextTab );
			Gwen::KeyboardFocus = pNextTab;
		}
	}

	return true;
}

void TabButton::UpdateColours()
{
	if ( !IsActive() )
	{
		SetImageAlpha( 0.5 );

		if ( IsDisabled() )		{ return SetTextColor( GetSkin()->Colors.Tab.Inactive.Disabled ); }

		if ( IsDepressed() )	{ return SetTextColor( GetSkin()->Colors.Tab.Inactive.Down ); }

		if ( IsHovered() )		{ return SetTextColor( GetSkin()->Colors.Tab.Inactive.Hover ); }

		return SetTextColor( GetSkin()->Colors.Tab.Inactive.Normal );
	}

	SetImageAlpha( 1.0 );

	if ( IsDisabled() )		{ return SetTextColor( GetSkin()->Colors.Tab.Active.Disabled ); }

	if ( IsDepressed() )	{ return SetTextColor( GetSkin()->Colors.Tab.Active.Down ); }

	if ( IsHovered() )		{ return SetTextColor( GetSkin()->Colors.Tab.Active.Hover ); }

	SetTextColor( GetSkin()->Colors.Tab.Active.Normal );
}

void TabButton::OnCloseButton(Controls::Base* control)
{
	auto page = GetPage();
	GetTabControl()->RemovePage(this);
	DelayedDelete();
	page->DelayedDelete();
}
