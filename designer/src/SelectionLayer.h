
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class SelectionLayer : public Controls::Base 
{
	GWEN_CONTROL( SelectionLayer, Controls::Base );

	virtual void OnMouseClickLeft( int x, int y, bool bDown );

	virtual void ClearSelection();
	virtual void AddSelection( Controls::Base* pControl );
	virtual void RemoveSelection(  Controls::Base* pControl );

	ControlList& GetSelected(){ return m_Selected; }

	Event::Caller onSelectionChanged;
	Event::Caller onPropertiesChanged;
	Event::Caller onHierachyChanged;

	protected:

		void OnCageMoving( Event::Info info );
		void OnControlDragged( Event::Info info );
		void OnCagePressed( Event::Info info );
		void OnDragStart();

		void SwitchCage( Controls::Base* pControl, Controls::Base* pTo );

		ControlList	m_Selected;

};