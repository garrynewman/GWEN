/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Utility.h"

using namespace Gwen;
using namespace Gwen::Controls;

GWEN_CONTROL_CONSTRUCTOR( Label )
{
	m_CreatedFont = NULL;
	m_Text = new ControlsInternal::Text( this );
	m_Text->SetFont( GetSkin()->GetDefaultFont() );
	SetMouseInputEnabled( false );
	SetBounds( 0, 0, 100, 10 );
	m_iAlign = Gwen::Pos::Left | Gwen::Pos::Top;
	//SetAlignment( Gwen::Pos::Left | Gwen::Pos::Top );
}

void Label::PreDelete( Gwen::Skin::Base* skin )
{
	if ( m_CreatedFont )
	{
		skin->ReleaseFont( m_CreatedFont );
		delete m_CreatedFont;
		m_CreatedFont = NULL;
		SetFont( NULL );
	}
}

void Label::PostLayout( Skin::Base* /*skin*/ )
{
	m_Text->Position( m_iAlign );
}

void Label::SetAlignment( int iAlign )
{
	if ( m_iAlign == iAlign ) { return; }

	m_iAlign = iAlign;
	Invalidate();
}

int Label::GetAlignment()
{
	return m_iAlign;
}

void Label::SetText( const TextObject & str, bool bDoEvents )
{
	if ( m_Text->GetText() == str.GetUnicode() ) { return; }

	m_Text->SetString( str );
	Redraw();

	if ( bDoEvents )
	{ OnTextChanged(); }
}

void Label::SizeToContents()
{
	m_Text->SetPos( m_Padding.left, m_Padding.top );
	m_Text->RefreshSize();
	SetSize( m_Text->Width() + m_Padding.left + m_Padding.right, m_Text->Height() + m_Padding.top + m_Padding.bottom );
}

Gwen::Rect Label::GetCharacterPosition( int iChar )
{
	Gwen::Rect p = m_Text->GetCharacterPosition( iChar );
	p.x += m_Text->X();
	p.y += m_Text->Y();
	return p;
}

void Label::OnBoundsChanged( Gwen::Rect oldChildBounds )
{
	BaseClass::OnBoundsChanged( oldChildBounds );

	if ( m_Text->Wrap() )
	{
		m_Text->RefreshSize();
		Invalidate();
	}
}

void Label::SetFont( Gwen::UnicodeString strFacename, int iSize, bool bBold )
{
	if ( m_CreatedFont )
	{
		GetSkin()->ReleaseFont( m_CreatedFont );
		delete m_CreatedFont;
		m_CreatedFont = NULL;
		SetFont( NULL );
	}

	m_CreatedFont = new Gwen::Font();
	Debug::AssertCheck( m_CreatedFont != NULL, "Couldn't Create Font!" );
	m_CreatedFont->bold = bBold;
	m_CreatedFont->facename = strFacename;
	m_CreatedFont->size = iSize;
	SetFont( m_CreatedFont );
	m_Text->RefreshSize();
}
