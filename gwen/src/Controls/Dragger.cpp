/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Controls/Dragger.h"

using namespace Gwen;
using namespace Gwen::ControlsInternal;



GWEN_CONTROL_CONSTRUCTOR( Dragger )
{
	m_pTarget = NULL;
	SetMouseInputEnabled( true );
	m_bDepressed = false;
	m_bDoMove = true;
}

void Dragger::OnMouseClickLeft( int x, int y, bool bDown )
{
	if ( bDown )
	{
		m_bDepressed = true;

		if ( m_pTarget )
		{ m_HoldPos = m_pTarget->CanvasPosToLocal( Gwen::Point( x, y ) ); }

		Gwen::MouseFocus = this;
		onDragStart.Call( this );
		Redraw();
	}
	else
	{
		m_bDepressed = false;
		Gwen::MouseFocus = NULL;
		Redraw();
	}
}

void Dragger::OnMouseMoved( int x, int y, int deltaX, int deltaY )
{
	if ( !m_bDepressed ) { return; }

	if ( m_bDoMove && m_pTarget )
	{
		Gwen::Point p = Gwen::Point( x - m_HoldPos.x, y - m_HoldPos.y );

		// Translate to parent
		if ( m_pTarget->GetParent() )
		{ p = m_pTarget->GetParent()->CanvasPosToLocal( p ); }

		m_pTarget->MoveTo( p.x, p.y );
	}

	Gwen::Event::Information info;
	info.Point = Gwen::Point( deltaX, deltaY );
	onDragged.Call( this, info );
}

void Dragger::Render( Skin::Base* skin )
{
	//skin->DrawButton(this,false,false, false);
}

void Dragger::OnMouseDoubleClickLeft( int x, int y )
{
	onDoubleClickLeft.Call( this );
}
