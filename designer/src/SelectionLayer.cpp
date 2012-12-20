#include "SelectionLayer.h"
#include "Cage.h"
#include "Gwen/Util/ControlFactory.h"
#include "Utility.h"

GWEN_CONTROL_CONSTRUCTOR( SelectionLayer )
{
	SetShouldDrawBackground( true );
}

void SelectionLayer::ClearSelection()
{
	m_Selected.Clear();
	RemoveAllChildren();
	Redraw();
}

void SelectionLayer::AddSelection( Controls::Base* pControl )
{
	Cage* pCage = new Cage( this );
	pCage->Setup( pControl );
	pCage->onMoved.Add( this, &ThisClass::OnControlDragged );
	pCage->onPress.Add( this, &ThisClass::OnCagePressed );
	pCage->onMoving.Add( this, &ThisClass::OnCageMoving );
	pCage->onDragStart.Add( this, &ThisClass::OnDragStart );

	m_Selected.Add( pControl );

	Redraw();
}

void SelectionLayer::RemoveSelection( Controls::Base* pControl )
{
	m_Selected.Remove( pControl );

	for ( int i=0; i<NumChildren(); i++ )
	{
		Cage* pCage = gwen_cast<Cage>( GetChild( i ) );
		if  (!pCage ) continue;

		if ( pCage->Target() == pControl )
			pCage->DelayedDelete();
	}

	Redraw();
}

void SelectionLayer::OnMouseClickLeft( int x, int y, bool bDown )
{
	if ( !bDown ) return;

	Gwen::Point pPos = GetParent()->CanvasPosToLocal( Gwen::Point( x, y ) );
	
	SetMouseInputEnabled( false );

	Controls::Base* pChild = GetParent()->GetControlAt( pPos.x, pPos.y );
	Controls::Base* pCtrl = FindParentControlFactoryControl( pChild );

	SetMouseInputEnabled( true );

	bool bPanelsWereSelected = !m_Selected.list.empty();


	if ( !Gwen::Input::IsShiftDown() )
	{
		ClearSelection();
	}

	if ( pCtrl )
	{

		if ( pCtrl != pChild )
		{
			Gwen::ControlFactory::Base* pFactory = pCtrl->UserData.Get<Gwen::ControlFactory::Base*>( "ControlFactory" );

			if ( pFactory->ChildTouched( pCtrl, pChild ) )
				return;
		}

			if ( pCtrl == GetParent() )
			{
				//
				// We only select the canvas if no other panels were selected before
				//
				if ( !bPanelsWereSelected )
					AddSelection( pCtrl );
			}
			else
			{
				AddSelection( pCtrl );
			}
	}

	Event::Information info;
	info.ControlList = m_Selected;

	onSelectionChanged.Call( this, info );
}

void SelectionLayer::OnControlDragged( Event::Info info )
{
	for ( ControlList::List::const_iterator it = m_Selected.list.begin(); it != m_Selected.list.end(); ++it )
	{
		Controls::Base* pControl = (*it);
		pControl->MoveBy( info.Point.x, info.Point.y );
	}

	Event::Information inf;
		inf.ControlList = m_Selected;
	onPropertiesChanged.Call( this, inf );
}

void SelectionLayer::OnCagePressed( Event::Info info )
{
	Cage* pCage = gwen_cast<Cage>( info.ControlCaller );
	if ( !pCage ) return;

	RemoveSelection( pCage->Target() );
	pCage->DelayedDelete();	

	// Let everything else know the selection changed
	{
		Event::Information info;
		info.ControlList = m_Selected;
		onSelectionChanged.Call( this, info );
	}
}

void SelectionLayer::OnCageMoving( Event::Info info )
{
	//
	// If we have the canvas selected - then don't do anything.
	//
	if ( m_Selected.Contains( GetParent() ) )
		return;


	//
	// Convert the passed canvas pos to a pos local to the canvas
	//
	Gwen::Point pPos = GetParent()->CanvasPosToLocal( info.Point );
	
	// Hide all of the selected panels, and this selection layer
	{
		for ( ControlList::List::const_iterator it = m_Selected.list.begin(); it != m_Selected.list.end(); ++it )
		{
			(*it)->SetHidden( true );
		}

		SetHidden( true );
	}

	// Find out which control is under our cursor
	Controls::Base* pChild = GetParent()->GetControlAt( pPos.x, pPos.y );
	Controls::Base* pCtrl = FindParentControlFactoryControl( pChild );
		
	bool bHierachyChanged = false;

	// Show all the hidden panels
	{
		SetHidden( false );
		
		for ( ControlList::List::const_iterator it = m_Selected.list.begin(); it != m_Selected.list.end(); ++it )
		{
			(*it)->SetHidden( false );
			
			if ( (*it) == GetParent() ) continue;

			if ( !pCtrl ) continue;
			if ( !pCtrl->UserData.Exists( "ControlFactory" ) ) continue;

			Gwen::ControlFactory::Base* pFactory = pCtrl->UserData.Get<Gwen::ControlFactory::Base*>( "ControlFactory" );

			Controls::Base* pOldParent = (*it)->GetParent();

			// If the panel we're dragging doesn't have the parent thats underneath
			// then make it have it. Tweak positions so they're the same
			if ( pCtrl && pCtrl != (*it)->GetParent() )
			{
				Gwen::Point pPos = (*it)->LocalPosToCanvas();
				pFactory->AddChild( pCtrl, (*it), pCtrl->CanvasPosToLocal( info.Point ) );
				(*it)->SetPos( (*it)->GetParent()->CanvasPosToLocal( pPos ) );

				bHierachyChanged = bHierachyChanged || ( pOldParent != (*it)->GetParent() );
			}
		}
	}

	//
	// If any parents changed then fire an event to update the tree
	//
	if ( bHierachyChanged )
	{
		onHierachyChanged.Call( this );
	}

}

void SelectionLayer::OnDragStart()
{
	//
	// If shift dragging, duplicate the selected panels
	//
	if ( !Gwen::Input::IsShiftDown() ) return;

	ControlList NewList;

	for ( ControlList::List::const_iterator it = m_Selected.list.begin(); it != m_Selected.list.end(); ++it )
	{
		Gwen::ControlFactory::Base* pFactory = (*it)->UserData.Get<Gwen::ControlFactory::Base*>( "ControlFactory" );
		Controls::Base* pControl = ControlFactory::Clone( *it, pFactory );
		pControl->UserData.Set( "ControlFactory", pFactory );
		pControl->SetMouseInputEnabled( true );
		NewList.Add( pControl );

		SwitchCage( *it, pControl );
	}

	m_Selected = NewList;
}

void SelectionLayer::SwitchCage( Controls::Base* pControl, Controls::Base* pTo )
{
	for ( int i=0; i<NumChildren(); i++ )
	{
		Cage* pCage = gwen_cast<Cage>( GetChild( i ) );
		if  (!pCage ) continue;

		if ( pCage->Target() == pControl )
		{
			pCage->Setup( pTo );
		}
	}
}