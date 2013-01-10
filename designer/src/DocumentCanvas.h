
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class SelectionLayer;

class DocumentCanvas : public Controls::Base 
{
	GWEN_CONTROL( DocumentCanvas, Controls::Base );

	virtual void Render( Gwen::Skin::Base* skin );
	virtual void PostLayout( Skin::Base* skin );

	virtual void SelectControls( ControlList& list );

	virtual bool DragAndDrop_CanAcceptPackage( Gwen::DragAndDrop::Package* pPackage );
	virtual bool DragAndDrop_HandleDrop( Gwen::DragAndDrop::Package* pPackage, int x, int y );

	void Command( const Gwen::String& str );

	Event::Caller	onChildAdded;
	Event::Caller	onChildRemoved;
	Event::Caller	onSelectionChanged;
	Event::Caller	onPropertiesChanged;
	Event::Caller	onHierachyChanged;

	protected:

		void OnSelectionChanged( Event::Info info );
		void OnPropertiesChanged( Event::Info info );
		void OnHierachyChanged( Event::Info info );
		

		SelectionLayer*	m_SelectionLayer;
};