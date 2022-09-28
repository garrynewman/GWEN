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
	int item_height = 7;
	for ( Base::List::iterator it = m_InnerPanel->Children.begin(); it != m_InnerPanel->Children.end(); ++it )
	{
		Base* pChild = ( *it );

		if ( !pChild )
		{ continue; }
		
		auto msize = pChild->GetMinimumSize();
		msize.y += 5;	
		pChild->SetSize(msize);
		
		item_height = std::max(item_height, msize.y);
	}
	SetSize(Width(), item_height);
}

void MenuStrip::OnAddItem( MenuItem* item )
{
	item->Dock( Pos::Left );
	item->SetTextPadding( Padding( 5, 0, 5, 0 ) );
	item->SetPadding( Padding( 10, 0, 10, 0 ) );
	item->SizeToContents();
	item->SetOnStrip( true );
	item->onHoverEnter.Add( this, &Menu::OnHoverItem );
}

bool MenuStrip::ShouldHoverOpenMenu()
{
	return IsMenuOpen();
}
