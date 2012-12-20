
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class Cage : public Controls::Button 
{
	GWEN_CONTROL( Cage, Controls::Button );

		void Setup( Controls::Base* pControl );

		virtual void Render( Gwen::Skin::Base* skin );
		virtual void PostLayout( Skin::Base* skin );

		virtual void OnMouseMoved( int x, int y, int deltaX, int deltaY );
		virtual void SetDepressed( bool b );

		virtual void OnPress();

		virtual Controls::Base* Target() { return m_Control; }

		Event::Caller	onMoved;
		Event::Caller	onDragStart;
		Event::Caller	onMoving;

	protected:

		bool				m_bDragged;
		Controls::Base*		m_Control;
		int					m_iBorder;

		Point				m_DragPoint;

};