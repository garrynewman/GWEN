/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/ImagePanel.h"

using namespace Gwen;
using namespace Gwen::Controls;


GWEN_CONTROL_CONSTRUCTOR( Button )
{
	m_Image = NULL;
	m_bDepressed = false;
	m_bCenterImage = false;
	m_bToggleStatus = false;
	SetSize( 100, 20 );
	SetMouseInputEnabled( true );
	SetIsToggle( false );
	SetAlignment( Gwen::Pos::Center );
	SetTextPadding( Padding( 3, 0, 3, 0 ) );
	SetKeyboardInputEnabled( false );
	SetTabable( false );
}

void Button::Render( Skin::Base* skin )
{
	if ( !ShouldDrawBackground() ) { return; }

	bool bDrawDepressed = IsDepressed() && IsHovered();

	if ( IsToggle() ) { bDrawDepressed = bDrawDepressed || GetToggleState(); }

	bool bDrawHovered = IsHovered() && ShouldDrawHover();
	skin->DrawButton( this, bDrawDepressed, bDrawHovered, IsDisabled() );
}

void Button::OnMouseClickLeft( int /*x*/, int /*y*/, bool bDown )
{
	if ( IsDisabled() ) { return; }

	if ( bDown )
	{
		SetDepressed( true );
		Gwen::MouseFocus = this;
		onDown.Call( this );
	}
	else
	{
		if ( IsHovered() && m_bDepressed )
		{
			OnPress();
		}

		SetDepressed( false );
		Gwen::MouseFocus = NULL;
		onUp.Call( this );
	}
}

void Button::OnMouseClickRight( int /*x*/, int /*y*/, bool bDown )
{
	if ( IsDisabled() ) { return; }

	if ( bDown )
	{
		SetDepressed( true );
		Gwen::MouseFocus = this;
	}
	else
	{
		if ( IsHovered() && m_bDepressed )
		{
			OnRightPress();
		}

		SetDepressed( false );
		Gwen::MouseFocus = NULL;
	}
}


void Button::SetDepressed( bool b )
{
	if ( m_bDepressed == b ) { return; }

	m_bDepressed = b;
	Redraw();
}

void Button::OnRightPress()
{
	onRightPress.Call( this );
}

void Button::OnPress()
{
	if ( IsToggle() )
	{
		SetToggleState( !GetToggleState() );
	}

	onPress.Call( this );
}


void Button::SetImage( const TextObject & strName, bool bCenter, bool bMaintainAspectRatio )
{
	if ( strName.GetUnicode() == L"" )
	{
		if ( m_Image )
		{
			delete m_Image;
			m_Image = NULL;
		}

		return;
	}

	if ( !m_Image )
	{
		m_Image = new ImagePanel( this );
	}

	m_Image->SetImage( strName );
	m_Image->SizeToContents();
	if (bMaintainAspectRatio)
	{
		float aspect_ratio = m_Image->TextureWidth()/m_Image->TextureHeight();
		m_Image->SetSize(Height()*aspect_ratio-2, Height()-2);
	}
	m_Image->SetMargin( Margin( 2, 0, 2, 0 ) );
	m_bCenterImage = bCenter;
	// Ugh.
	Padding padding = GetTextPadding();
	padding.left = m_Image->Right() + 2;
	SetTextPadding( padding );
}

void Button::SetToggleState( bool b )
{
	if ( m_bToggleStatus == b ) { return; }

	m_bToggleStatus = b;
	onToggle.Call( this );

	if ( m_bToggleStatus )
	{
		onToggleOn.Call( this );
	}
	else
	{
		onToggleOff.Call( this );
	}

	Redraw();
}

void Button::SizeToContents()
{
	BaseClass::SizeToContents();

	if ( m_Image )
	{
		int height = m_Image->Height() + 4;

		if ( Height() < height )
		{
			SetHeight( height );
		}
	}
}

bool Button::OnKeySpace( bool bDown )
{
	if ( bDown )
	{
		OnPress();
	}

	return true;
}

void Button::AcceleratePressed()
{
	OnPress();
}

void Button::UpdateColours()
{
	if ( IsDisabled() )		{ return SetTextColor( GetSkin()->Colors.Button.Disabled ); }

	if ( IsDepressed() || GetToggleState() )	{ return SetTextColor( GetSkin()->Colors.Button.Down ); }

	if ( IsHovered() )		{ return SetTextColor( GetSkin()->Colors.Button.Hover ); }

	SetTextColor( GetSkin()->Colors.Button.Normal );
}

void Button::PostLayout( Skin::Base* pSkin )
{
	BaseClass::PostLayout( pSkin );

	if ( m_Image )
	{
		if ( m_bCenterImage )
		{ m_Image->Position( Pos::Center ); }
		else
		{ m_Image->Position( Pos::Left | Pos::CenterV ); }
	}
}

void Button::OnMouseDoubleClickLeft( int x, int y )
{
	if ( IsDisabled() ) { return; }

	OnMouseClickLeft( x, y, true );
	onDoubleClick.Call( this );
}

void Button::SetImageAlpha( float f )
{
	if ( !m_Image ) { return; }

	m_Image->SetDrawColor( Gwen::Color( 255, 255, 255, 255.0f * f ) );
}

void Button::SetAction( Event::Handler* pObject, Handler::FunctionWithInformation pFunction, void* data )
{
	onPress.Add( pObject, pFunction, data );
}
