/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Controls/MenuItem.h"
#include "Gwen/Skin.h"

using namespace Gwen;
using namespace Gwen::Controls;

class GWEN_EXPORT RightArrow : public Controls::Base
{
	public:

		GWEN_CONTROL_INLINE( RightArrow, Controls::Base )
		{
			SetMouseInputEnabled( false );
		}

		void Render( Skin::Base* skin )
		{
			skin->DrawMenuRightArrow( this );
		}
};

GWEN_CONTROL_CONSTRUCTOR( MenuItem )
{
	m_Menu = NULL;
	m_bOnStrip = false;
	m_SubmenuArrow = NULL;
	m_Accelerator = NULL;
	SetTabable( false );
	SetCheckable( false );
    m_bChecked = false;
	//SetChecked( false );
}

MenuItem::~MenuItem()
{
}

void MenuItem::OnScaleChanged()
{
	BaseClass::SizeToContents();
	if (m_Accelerator)
	{
		m_Accelerator->SizeToContents();
	}
}

void MenuItem::Render( Skin::Base* skin )
{
	skin->DrawMenuItem( this, IsMenuOpen(), m_bCheckable ? m_bChecked : false, IsHovered() );

	// HACK!
	if ( m_Accelerator )
	{
		m_Accelerator->SetTextColorOverride( TextColor() );
	}
}

void MenuItem::Layout( Skin::Base* skin )
{
	if ( m_SubmenuArrow )
	{
		m_SubmenuArrow->Position( Pos::Right | Pos::CenterV, 4, 0 );
	}

	BaseClass::Layout( skin );
}

Menu* MenuItem::GetMenu()
{
	if ( !m_Menu )
	{
		m_Menu = new Menu( GetCanvas() );
		m_Menu->SetHidden( true );

		if ( !m_bOnStrip )
		{
			m_SubmenuArrow = new RightArrow( this );
			m_SubmenuArrow->SetSize( 15, 15 );
		}

		Invalidate();
	}

	return m_Menu;
}

void MenuItem::SetChecked( bool bCheck )
{
	if ( bCheck == m_bChecked )
	{ return; }

	m_bChecked = bCheck;
	onCheckChange.Call( this );

	if ( bCheck )
	{ onChecked.Call( this ); }
	else
	{ onUnChecked.Call( this ); }
}

void MenuItem::OnPress()
{
	if ( m_Menu )
	{
		ToggleMenu();
	}
	else if ( !m_bOnStrip )
	{
		SetChecked( !GetChecked() );
		onMenuItemSelected.Call( this );
		GetCanvas()->CloseMenus();
	}

	BaseClass::OnPress();
}

void MenuItem::ToggleMenu()
{
	if ( IsMenuOpen() ) { CloseMenu(); }
	else { OpenMenu(); }
}

bool MenuItem::IsMenuOpen()
{
	if ( !m_Menu ) { return false; }

	return !m_Menu->Hidden();
}

Gwen::Point MenuItem::GetMinimumSize()
{
	int w = 0;
	if (m_Accelerator)
	{
		w += 16;
		w += m_Accelerator->TextWidth();
	}
	if (m_SubmenuArrow)
	{
		w += m_SubmenuArrow->Width();
	}
	w += TextWidth();
	
	auto padding = GetPadding();
	w += padding.left + padding.right;
	
	auto tpadding = GetTextPadding();
	return Gwen::Point(w, TextHeight() + padding.top + padding.bottom + tpadding.top + tpadding.bottom);
}

void MenuItem::OpenMenu()
{
	if ( !m_Menu ) { return; }

	m_Menu->SetHidden( false );
	m_Menu->BringToFront();
	Gwen::Point p = LocalPosToCanvas( Gwen::Point( 0, 0 ) );

	// Strip menus open downwards
	if ( m_bOnStrip )
	{
		m_Menu->SetPos( p.x, p.y + Height() + 1 );
	}
	// Submenus open sidewards
	else
	{
		m_Menu->SetPos( p.x + Width(), p.y );
	}

	// TODO: Option this.
	// TODO: Make sure on screen, open the other side of the
	// parent if it's better...
}

void MenuItem::CloseMenu()
{
	if ( !m_Menu ) { return; }

	m_Menu->Close();
	m_Menu->CloseAll();
}

void MenuItem::SetAccelerator( const TextObject & strAccelerator )
{
	if ( m_Accelerator )
	{
		m_Accelerator->DelayedDelete();
		m_Accelerator = NULL;
	}

	if ( strAccelerator.GetUnicode() == L"" )
	{ return; }

	m_Accelerator = new Controls::Label( this );
	m_Accelerator->Dock( Pos::Right );
	m_Accelerator->SetAlignment( Pos::Right | Pos::CenterV );
	m_Accelerator->SetText( strAccelerator );
	m_Accelerator->SetMargin( Margin( 0, 0, 16, 0 ) );
	// TODO.
}

void MenuItem::SizeToContents()
{
	BaseClass::SizeToContents();

	if ( m_Accelerator )
	{
		m_Accelerator->SizeToContents();
		SetWidth( Width() + m_Accelerator->Width() );
	}
}
