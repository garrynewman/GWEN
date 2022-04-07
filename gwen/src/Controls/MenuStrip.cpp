/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Controls/MenuStrip.h"
#include "Gwen/Skin.h"

using namespace Gwen;
using namespace Gwen::Controls;

GWEN_CONTROL_CONSTRUCTOR( MenuStrip )
{
	m_ItemHeight = 17;
	SetBounds( 0, 0, 200, 22 );
	Dock( Pos::Top );
	m_InnerPanel->SetPadding( Padding( 5, 0, 0, 0 ) );
}

void MenuStrip::Render( Skin::Base* skin )
{
	skin->DrawMenuStrip( this );
}

void MenuStrip::Layout( Skin::Base* skin )
{
	// Size to the font
	SetSize(Width(), m_ItemHeight + 10);
}

void MenuStrip::OnAddItem( MenuItem* item )
{
	item->Dock( Pos::Left );
	item->SetTextPadding( Padding( 5, 0, 5, 0 ) );
	item->SetPadding( Padding( 10, 0, 10, 0 ) );
	item->SizeToContents();
	item->SetOnStrip( true );
	item->onHoverEnter.Add( this, &Menu::OnHoverItem );
	m_ItemHeight = item->Height();
}

bool MenuStrip::ShouldHoverOpenMenu()
{
	return IsMenuOpen();
}
