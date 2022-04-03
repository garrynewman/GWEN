/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/DragAndDrop.h"
#include "Gwen/Utility.h"
#include "Gwen/Platform.h"

using namespace Gwen;
using namespace Gwen::DragAndDrop;

DragAndDrop::Package* DragAndDrop::CurrentPackage = NULL;
Gwen::Controls::Base* DragAndDrop::HoveredControl = NULL;
Gwen::Controls::Base* DragAndDrop::SourceControl = NULL;

static Gwen::Controls::Base* LastPressedControl = NULL;
static Gwen::Controls::Base* NewHoveredControl = NULL;
static Gwen::Point LastPressedPos;

void DragAndDrop::ControlDeleted( Gwen::Controls::Base* pControl )
{
	if ( SourceControl == pControl )
	{
		SourceControl = NULL;
		CurrentPackage = NULL;
		HoveredControl = NULL;
		LastPressedControl = NULL;
	}

	if ( LastPressedControl == pControl )
	{ LastPressedControl = NULL; }

	if ( HoveredControl == pControl )
	{ HoveredControl = NULL; }

	if ( NewHoveredControl == pControl )
	{ NewHoveredControl = NULL; }
}

// In global coordinates
static int m_iMouseX = 0;
static int m_iMouseY = 0;

bool DragAndDrop::Start( Gwen::Controls::Base* pControl, Package* pPackage )
{
	if ( CurrentPackage )
	{
		return false;
	}

	CurrentPackage = pPackage;
	SourceControl = pControl;
	return true;
}

bool OnDrop( int x, int y )
{
	bool bSuccess = false;

	if ( DragAndDrop::HoveredControl )
	{
		Gwen::Point wpos = DragAndDrop::HoveredControl->GetCanvas()->WindowPosition();
		int cx = x - wpos.x;
		int cy = y - wpos.y;
		DragAndDrop::HoveredControl->DragAndDrop_HoverLeave( DragAndDrop::CurrentPackage );
		bSuccess = DragAndDrop::HoveredControl->DragAndDrop_HandleDrop( DragAndDrop::CurrentPackage, cx, cy );
	}

	// Report back to the source control, to tell it if we've been successful.
	Gwen::Point wpos = DragAndDrop::SourceControl->GetCanvas()->WindowPosition();
	int cx = x - wpos.x;
	int cy = y - wpos.y;
	DragAndDrop::SourceControl->DragAndDrop_EndDragging( bSuccess, cx, cy );
	DragAndDrop::SourceControl->Redraw();
	DragAndDrop::CurrentPackage = NULL;
	DragAndDrop::SourceControl = NULL;
	return true;
}

bool DragAndDrop::OnMouseButton( Gwen::Controls::Base* pHoveredControl, int x, int y, bool bDown )
{	
	if ( !bDown )
	{
		LastPressedControl = NULL;

		// Not carrying anything, allow normal actions
		if ( !CurrentPackage )
		{ return false; }

		// We were carrying something, drop it.
		OnDrop( x, y );
		return true;
	}

	if ( !pHoveredControl ) { return false; }

	if ( !pHoveredControl->DragAndDrop_Draggable() ) { return false; }

	// Store the last clicked on control. Don't do anything yet,
	// we'll check it in OnMouseMoved, and if it moves further than
	// x pixels with the mouse down, we'll start to drag.
	LastPressedPos = Gwen::Point( x, y );
	LastPressedControl = pHoveredControl;
	return false;
}

bool ShouldStartDraggingControl( int x, int y )
{
	// We're not holding a control down..
	if ( !LastPressedControl ) { return false; }

	// Not been dragged far enough
	int iLength = abs( x - LastPressedPos.x ) + abs( y - LastPressedPos.y );

	if ( iLength < 5 ) { return false; }

	// Create the dragging package
	Gwen::Point wpos = LastPressedControl->GetCanvas()->WindowPosition();
	int lpcx = LastPressedPos.x - wpos.x;
	int lpcy = LastPressedPos.y - wpos.y;
	DragAndDrop::CurrentPackage = LastPressedControl->DragAndDrop_GetPackage( lpcx, lpcy );

	// We didn't create a package!
	if ( !DragAndDrop::CurrentPackage )
	{
		LastPressedControl = NULL;
		DragAndDrop::SourceControl = NULL;
		return false;
	}

	// Now we're dragging something!
	DragAndDrop::SourceControl = LastPressedControl;
	Gwen::MouseFocus = NULL;
	LastPressedControl = NULL;
	DragAndDrop::CurrentPackage->drawcontrol = NULL;

	// Some controls will want to decide whether they should be dragged at that moment.
	// This function is for them (it defaults to true)
	if ( !DragAndDrop::SourceControl->DragAndDrop_ShouldStartDrag() )
	{
		DragAndDrop::SourceControl = NULL;
		DragAndDrop::CurrentPackage = NULL;
		return false;
	}

	wpos = DragAndDrop::SourceControl->GetCanvas()->WindowPosition();
	lpcx = LastPressedPos.x - wpos.x;
	lpcy = LastPressedPos.y - wpos.y;
	DragAndDrop::SourceControl->DragAndDrop_StartDragging( DragAndDrop::CurrentPackage, lpcx, lpcy );
	return true;
}

void UpdateHoveredControl( Gwen::Controls::Base* pCtrl, int x, int y )
{
	//
	// We use this global variable to represent our hovered control
	// That way, if the new hovered control gets deleted in one of the
	// Hover callbacks, we won't be left with a hanging pointer.
	// This isn't ideal - but it's minimal.
	//
	NewHoveredControl = pCtrl;

	// Nothing to change..
	if ( DragAndDrop::HoveredControl == NewHoveredControl ) { return; }

	// We changed - tell the old hovered control that it's no longer hovered.
	if ( DragAndDrop::HoveredControl && DragAndDrop::HoveredControl != NewHoveredControl )
	{ DragAndDrop::HoveredControl->DragAndDrop_HoverLeave( DragAndDrop::CurrentPackage ); }

	// If we're hovering where the control came from, just forget it.
	// By changing it to NULL here we're not going to show any error cursors
	// it will just do nothing if you drop it.
	if ( NewHoveredControl == DragAndDrop::SourceControl )
	{ NewHoveredControl = NULL; }

	// Check to see if the new potential control can accept this type of package.
	// If not, ignore it and show an error cursor.
	while ( NewHoveredControl && !NewHoveredControl->DragAndDrop_CanAcceptPackage( DragAndDrop::CurrentPackage ) )
	{
		// We can't drop on this control, so lets try to drop
		// onto its parent..
		NewHoveredControl = NewHoveredControl->GetParent();

		// Its parents are dead. We can't drop it here.
		// Show the NO WAY cursor.
		if ( !NewHoveredControl )
		{
			Platform::SetCursor( CursorType::No );
		}
	}

	// Become out new hovered control
	DragAndDrop::HoveredControl = NewHoveredControl;

	// If we exist, tell us that we've started hovering.
	if ( DragAndDrop::HoveredControl )
	{
		Gwen::Point wpos = DragAndDrop::HoveredControl->GetCanvas()->WindowPosition();
		int cx = x - wpos.x;
		int cy = y - wpos.y;
		DragAndDrop::HoveredControl->DragAndDrop_HoverEnter( DragAndDrop::CurrentPackage, cx, cy );
	}

	NewHoveredControl = NULL;
}

void DragAndDrop::OnMouseMoved( Gwen::Controls::Base* pHoveredControl, int x, int y )
{	
	// Always keep these up to date, they're used to draw the dragged control.
	m_iMouseX = x;
	m_iMouseY = y;

	// If we're not carrying anything, then check to see if we should
	// pick up from a control that we're holding down. If not, then forget it.
	if ( !CurrentPackage && !ShouldStartDraggingControl( x, y ) )
	{ return; }

	// Make sure the canvas redraws when we move
	if ( CurrentPackage && CurrentPackage->drawcontrol )
	{ CurrentPackage->drawcontrol->Redraw(); }

	// Swap to this new hovered control and notify them of the change.
	UpdateHoveredControl( pHoveredControl, x, y );

	if ( !HoveredControl ) { return; }

	// Update the hovered control every mouse move, so it can show where
	// the dropped control will land etc..
	Gwen::Point wpos = HoveredControl->GetCanvas()->WindowPosition();
	int cx = x - wpos.x;
	int cy = y - wpos.y;
	
	HoveredControl->DragAndDrop_Hover( CurrentPackage, cx, cy );
	// Override the cursor - since it might have been set my underlying controls
	// Ideally this would show the 'being dragged' control. TODO
	Platform::SetCursor( CursorType::Normal );
	pHoveredControl->Redraw();
}

void DragAndDrop::RenderOverlay( Gwen::Controls::Canvas* canvas, Skin::Base* skin )
{
	if ( !CurrentPackage ) { return; }

	if ( !CurrentPackage->drawcontrol ) { return; }
	
	if ( canvas != CurrentPackage->drawcontrol->GetCanvas() ) { return; }
	
	// get mouse coords in local
	Gwen::Point wpos = canvas->WindowPosition();
	int mx = m_iMouseX - wpos.x;
	int my = m_iMouseY - wpos.y;
	
	Gwen::Point pntOld = skin->GetRender()->GetRenderOffset();
	skin->GetRender()->AddRenderOffset( Gwen::Rect( mx - SourceControl->X() - CurrentPackage->holdoffset.x, my - SourceControl->Y() - CurrentPackage->holdoffset.y, 0, 0 ) );
	CurrentPackage->drawcontrol->DoRender( skin );
	skin->GetRender()->SetRenderOffset( pntOld );
}
