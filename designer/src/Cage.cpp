
#include "Cage.h"

GWEN_CONTROL_CONSTRUCTOR( Cage )
{
	m_Control = NULL;
	m_iBorder = 5;
}

void Cage::Render( Gwen::Skin::Base* skin )
{
	Rect bounds = GetRenderBounds();

	bounds.x += m_iBorder;
	bounds.y += m_iBorder;
	bounds.w -= m_iBorder * 2;
	bounds.h -= m_iBorder * 2;

	skin->GetRender()->SetDrawColor( Color( 255, 255, 255, 100 ) );
	skin->GetRender()->DrawFilledRect( bounds );

	skin->GetRender()->SetDrawColor( Color( 20, 150, 255, 255 ) );
	skin->GetRender()->DrawLinedRect( bounds );
	bounds.x += 1; bounds.y += 1; bounds.w -= 2; bounds.h -= 2; 
	skin->GetRender()->DrawLinedRect( bounds );
}

void Cage::PostLayout( Skin::Base* skin )
{
	if ( !m_Control ) return;

	Point canvaspos = m_Control->LocalPosToCanvas();
	Point parentpos = GetParent()->CanvasPosToLocal( canvaspos );

	parentpos.x -= m_iBorder;
	parentpos.y -= m_iBorder;

	SetPos( parentpos );

	Point size = m_Control->GetSize();
	size.x += m_iBorder * 2;
	size.y += m_iBorder * 2;

	SetSize( size );
}

void Cage::Setup( Controls::Base* pControl )
{
	m_Control = pControl;
}

void Cage::SetDepressed( bool b )
{
	BaseClass::SetDepressed( b );

	m_DragPoint = m_Control->GetPos();
	m_bDragged = false;
}

void Cage::OnMouseMoved( int x, int y, int deltaX, int deltaY )
{
	if ( !IsDepressed() ) return;

	Controls::Base*	pControlParent = m_Control->GetParent();
	Point pntRemainder = m_DragPoint - m_Control->GetPos();

	//
	// This event is used by the SelectionLayer to scan
	// and change the parent - if appropriate.
	//
	{
		Event::Information info;

		info.Point = Point( x, y );
		onMoving.Call( this, info );
	}

	//
	// If the parent changed then fix up the drag position
	//
	if ( pControlParent != m_Control->GetParent() )
	{
		m_DragPoint = m_Control->GetPos() + pntRemainder;

	}

	if ( !m_bDragged )
	{
		Event::Information info;
		onDragStart.Call( this, info );
	}
	m_bDragged = true;

	m_DragPoint += Point( deltaX, deltaY );

	Point pos = m_DragPoint;

	pos.x = ((int)((float)pos.x / 10.0f)) * 10;
	pos.y = ((int)((float)pos.y / 10.0f)) * 10;

	pos -= m_Control->GetPos();

	{
		Event::Information info;
		info.Point = pos;
		onMoved.Call( this, info );
	}
}

void Cage::OnPress()
{
	if ( IsToggle() )
	{
		SetToggleState( !GetToggleState() );
	}

	if ( !m_bDragged )
		onPress.Call( this );
}