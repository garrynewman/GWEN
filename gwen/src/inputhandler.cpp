/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/InputHandler.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/DragAndDrop.h"
#include "Gwen/Hook.h"
#include "Gwen/Platform.h"
#include "Gwen/ToolTip.h"

#define DOUBLE_CLICK_SPEED 0.5f
#define MAX_MOUSE_BUTTONS 5

using namespace Gwen;


struct Action
{
	unsigned char type;

	int x, y;
	Gwen::UnicodeChar chr;
};

static const float KeyRepeatRate = 0.03f;
static const float KeyRepeatDelay = 0.3f;

struct t_KeyData
{
	t_KeyData()
	{
		for ( int i = 0; i < Gwen::Key::Count; i++ )
		{
			KeyState[i] = false;
			NextRepeat[i] = 0;
		}

		Target = NULL;
		LeftMouseDown = false;
		RightMouseDown = false;
	}

	bool KeyState[ Gwen::Key::Count ];
	float NextRepeat[ Gwen::Key::Count ];
	Controls::Base* Target;
	bool LeftMouseDown;
	bool RightMouseDown;

} KeyData;

Gwen::Point	MousePosition;

static float		g_fLastClickTime[MAX_MOUSE_BUTTONS];
static Gwen::Point	g_pntLastClickPos;

static std::map<Controls::Canvas*, bool> canvases;

enum
{
	ACT_MOUSEMOVE,
	ACT_MOUSEBUTTON,
	ACT_CHAR,
	ACT_MOUSEWHEEL,
	ACT_KEYPRESS,
	ACT_KEYRELEASE,
	ACT_MESSAGE
};

void UpdateHoveredControl(Gwen::Controls::Canvas* hovered_canv)
{
	// now send it to each canvas in their own coords
	Controls::Base* pHovered = 0;
	
	if (hovered_canv)
	{
		auto pos = MousePosition - hovered_canv->WindowPosition();
		float scale = hovered_canv->Scale();
		pHovered = hovered_canv->GetControlAt(pos.x/scale, pos.y/scale);
	}
	if (!pHovered)
	{
		for (auto canv: canvases)
		{
			// todo go to local coords
			auto pos = MousePosition - canv.first->WindowPosition();
			float scale = canv.first->Scale();
			pHovered = canv.first->GetControlAt( pos.x/scale, pos.y/scale );
		
			if (pHovered)
			{
				break;
			}
		}
	}
	
	if ( pHovered != Gwen::HoveredControl )
	{
		if ( Gwen::HoveredControl )
		{
			Controls::Base* OldHover = Gwen::HoveredControl;
			Gwen::HoveredControl = NULL;
			OldHover->OnMouseLeave();
		}

		Gwen::HoveredControl = pHovered;

		if ( Gwen::HoveredControl )
		{
			Gwen::HoveredControl->OnMouseEnter();
		}
	}

	if ( Gwen::MouseFocus )// && Gwen::MouseFocus->GetCanvas() == pInCanvas )
	{
		if ( Gwen::HoveredControl )
		{
			Controls::Base* OldHover = Gwen::HoveredControl;
			Gwen::HoveredControl = NULL;
			OldHover->Redraw();
		}

		Gwen::HoveredControl = Gwen::MouseFocus;
	}
}

bool FindKeyboardFocus( Controls::Base* pControl )
{
	if ( !pControl ) { return false; }

	if ( pControl->GetKeyboardInputEnabled() )
	{
		//Make sure none of our children have keyboard focus first - todo recursive
		for ( Controls::Base::List::iterator iter = pControl->Children.begin(); iter != pControl->Children.end(); ++iter )
		{
			Controls::Base* pChild = *iter;

			if ( pChild == Gwen::KeyboardFocus )
			{ return false; }
		}

		pControl->Focus();
		return true;
	}

	return FindKeyboardFocus( pControl->GetParent() );
}

void Gwen::Input::RegisterCanvas( Controls::Canvas* pCanvas )
{
	canvases[pCanvas] = true;
}

void Gwen::Input::RemoveCanvas( Controls::Canvas* pCanvas )
{
	canvases.erase(pCanvas);
}

Gwen::Point Gwen::Input::GetMousePosition()
{
	return MousePosition;
}


static Gwen::Controls::WindowCanvas* hovered_canvas = 0;

void Gwen::Input::UpdateHovered()
{
	// make sure hovered canvas is valid, it could have been deleted
	bool found = false;
	for (auto canv: canvases)
	{
		if (hovered_canvas == canv.first)
		{
			found = true;
			break;
		}
	}
	UpdateHoveredControl(found ? hovered_canvas : 0);
}

void Gwen::Input::OnThink()
{
	if ( Gwen::MouseFocus && !Gwen::MouseFocus->Visible() )
	{ Gwen::MouseFocus = NULL; }

	if ( Gwen::KeyboardFocus && ( !Gwen::KeyboardFocus->Visible() ||  !KeyboardFocus->GetKeyboardInputEnabled() ) )
	{ Gwen::KeyboardFocus = NULL; }

	if ( !KeyboardFocus ) { return; }

	float fTime = Gwen::Platform::GetTimeInSeconds();

	//
	// Simulate Key-Repeats
	//
	for ( int i = 0; i < Gwen::Key::Count; i++ )
	{
		if ( KeyData.KeyState[i] && KeyData.Target != KeyboardFocus )
		{
			KeyData.KeyState[i] = false;
			continue;
		}

		if ( KeyData.KeyState[i] && fTime > KeyData.NextRepeat[i] )
		{
			KeyData.NextRepeat[i] = Gwen::Platform::GetTimeInSeconds() + KeyRepeatRate;

			if ( KeyboardFocus )
			{
				KeyboardFocus->OnKeyPress( i );
			}
		}
	}
}

bool Gwen::Input::IsKeyDown( int iKey )
{
	return KeyData.KeyState[ iKey ];
}

bool Gwen::Input::IsLeftMouseDown()
{
	return KeyData.LeftMouseDown;
}

bool Gwen::Input::IsRightMouseDown()
{
	return KeyData.RightMouseDown;
}

bool Gwen::Input::OnMouseMoved( int x, int y, int deltaX, int deltaY, void* platform_window )
{
	MousePosition.x = x;
	MousePosition.y = y;
	
	// find the hovered window
	hovered_canvas = 0;
	for (auto canv: canvases)
	{
		Gwen::Controls::WindowCanvas* wcanv = dynamic_cast<Gwen::Controls::WindowCanvas*>(canv.first);
		if (wcanv && wcanv->GetWindow() == platform_window)
		{
			hovered_canvas = wcanv;
			break;
		}
	}
	
	//okay, lets have mouse position be global, then each canvas has an offset relative to global used to get local
	UpdateHoveredControl(hovered_canvas);
	//or could have one of these per canvas
	
	// now send it to each canvas in their own coords
	for (auto canv: canvases)
	{
		auto wpos = canv.first->WindowPosition();
		float scale = canv.first->Scale();
		canv.first->OnMouseMoved((x - wpos.x)/scale, (y - wpos.y)/scale, deltaX, deltaY);
	}
	
	if (Gwen::HoveredControl == 0)
	{
		return false;
	}

	if ( ToolTip::TooltipActive() )
	{
		ToolTip::Reset();
	}
	
	float fScale = Gwen::HoveredControl->GetCanvas()->Scale();
	auto wpos = Gwen::HoveredControl->GetCanvas()->WindowPosition();
	Gwen::HoveredControl->OnMouseMoved((x - wpos.x)/fScale, (y - wpos.y)/fScale, deltaX/fScale, deltaY/fScale);
	Gwen::HoveredControl->UpdateCursor();
	DragAndDrop::OnMouseMoved(Gwen::HoveredControl, x, y);// this one takes in global coords
	
	return true;
}

bool Gwen::Input::OnMouseButton( int iMouseButton, bool bDown )
{
	// If we click on a control that isn't a menu we want to close
	// all the open menus. Menus are children of the canvas.
	if ( bDown && ( !Gwen::HoveredControl || !Gwen::HoveredControl->IsMenuComponent() ) )
	{
		for (auto canv: canvases)
		{
			canv.first->CloseMenus();
		}
	}

	if ( !Gwen::HoveredControl )
	{
		// special case for drag and drop off screen
		if (iMouseButton == 0)
		{
			if ( iMouseButton == 0 ) { KeyData.LeftMouseDown = bDown; }
			if ( DragAndDrop::OnMouseButton( Gwen::HoveredControl, MousePosition.x, MousePosition.y, bDown ) )
			{ return true; }
		}
		return false;
	}

	//if ( Gwen::HoveredControl->GetCanvas() != pCanvas ) { return false; }

	if ( !Gwen::HoveredControl->Visible() ) { return false; }

	// todo probably need to keep this functionality
	//if ( Gwen::HoveredControl == pCanvas ) { return false; }

	if ( iMouseButton >= MAX_MOUSE_BUTTONS )
	{ return false; }

	if ( iMouseButton == 0 )		{ KeyData.LeftMouseDown = bDown; }
	else if ( iMouseButton == 1 )	{ KeyData.RightMouseDown = bDown; }

	// Double click.
	// Todo: Shouldn't double click if mouse has moved significantly
	bool bIsDoubleClick = false;

	if ( bDown &&
			g_pntLastClickPos.x == MousePosition.x &&
			g_pntLastClickPos.y == MousePosition.y &&
			( Gwen::Platform::GetTimeInSeconds() - g_fLastClickTime[ iMouseButton ] ) < DOUBLE_CLICK_SPEED )
	{
		bIsDoubleClick = true;
	}

	if ( bDown && !bIsDoubleClick )
	{
		g_fLastClickTime[ iMouseButton ] = Gwen::Platform::GetTimeInSeconds();
		g_pntLastClickPos = MousePosition;
	}

	if ( bDown )
	{
		if ( !FindKeyboardFocus( Gwen::HoveredControl ) )
		{
			if ( Gwen::KeyboardFocus )
			{ Gwen::KeyboardFocus->Blur(); }
		}
	}

	Gwen::HoveredControl->UpdateCursor();

	// This tells the child it has been touched, which
	// in turn tells its parents, who tell their parents.
	// This is basically so that Windows can pop themselves
	// to the top when one of their children have been clicked.
	if ( bDown )
	{ Gwen::HoveredControl->Touch(); }

#ifdef GWEN_HOOKSYSTEM

	if ( bDown )
	{
		if ( Hook::CallHook( &Hook::BaseHook::OnControlClicked, Gwen::HoveredControl, MousePosition.x, MousePosition.y ) )
		{ return true; }
	}

#endif

	Gwen::Point wpos = Gwen::HoveredControl->GetCanvas()->WindowPosition();
	float scale = Gwen::HoveredControl->GetCanvas()->Scale();
	int cx = (MousePosition.x - wpos.x)/scale;
	int cy = (MousePosition.y - wpos.y)/scale;

	switch ( iMouseButton )
	{
		case 0:
			{
				if ( DragAndDrop::OnMouseButton( Gwen::HoveredControl, MousePosition.x, MousePosition.y, bDown ) )
				{ return true; }

				if ( bIsDoubleClick )	{ Gwen::HoveredControl->OnMouseDoubleClickLeft( cx, cy ); }
				else					{ Gwen::HoveredControl->OnMouseClickLeft( cx, cy, bDown ); }

				return true;
			}

		case 1:
			{
				if ( bIsDoubleClick )	{ Gwen::HoveredControl->OnMouseDoubleClickRight( cx, cy ); }
				else					{ Gwen::HoveredControl->OnMouseClickRight( cx, cy, bDown ); }

				return true;
			}
	}

	return false;
}

bool Gwen::Input::HandleAccelerator( Gwen::UnicodeChar chr )
{
	//Build the accelerator search string
	Gwen::UnicodeString accelString;

	if ( Gwen::Input::IsControlDown() )
	{ accelString += L"CTRL+"; }

	if ( Gwen::Input::IsShiftDown() )
	{ accelString += L"SHIFT+"; }

	if (chr >= 0xE000 && chr < 0xF8FF)
	{
		chr -= 0xE000;
		if (chr >= Gwen::Key::F1)
		{
			int num = chr - Gwen::Key::F1 + 1;
			accelString += L"F" + std::to_wstring(num);
		}
	}
	else
	{
		chr = towupper(chr);
		accelString += chr;
	}

	//Debug::Msg("Accelerator string :%S\n", accelString.c_str());

	if ( Gwen::KeyboardFocus && Gwen::KeyboardFocus->HandleAccelerator( accelString ) )
	{ return true; }

	if ( Gwen::MouseFocus && Gwen::MouseFocus->HandleAccelerator( accelString ) )
	{ return true; }

	for (auto canvas: canvases)
	{
		if ( canvas.first->HandleAccelerator( accelString ) )
		{ return true; }
	}

	return false;
}

bool Gwen::Input::DoSpecialKeys( Controls::Base* pCanvas, Gwen::UnicodeChar chr )
{
	if ( !Gwen::KeyboardFocus ) { return false; }

	if ( Gwen::KeyboardFocus->GetCanvas() != pCanvas ) { return false; }

	if ( !Gwen::KeyboardFocus->Visible() ) { return false; }

	if ( !Gwen::Input::IsControlDown() ) { return false; }

	if ( chr == L'C' || chr == L'c' )
	{
		Gwen::KeyboardFocus->OnCopy( NULL );
		return true;
	}

	if ( chr == L'V' || chr == L'v' )
	{
		Gwen::KeyboardFocus->OnPaste( NULL );
		return true;
	}

	if ( chr == L'X' || chr == L'x' )
	{
		Gwen::KeyboardFocus->OnCut( NULL );
		return true;
	}

	if ( chr == L'A' || chr == L'a' )
	{
		Gwen::KeyboardFocus->OnSelectAll( NULL );
		return true;
	}

	return false;
}

bool Gwen::Input::OnKeyEvent( int iKey, bool bDown )
{
	if ( iKey <= Gwen::Key::Invalid ) { return false; }

	if ( iKey >= Gwen::Key::Count ) { return false; }

	if (bDown)
		if (Gwen::Input::HandleAccelerator(0xE000+iKey))
			return true;
	
	Gwen::Controls::Base* pTarget = Gwen::KeyboardFocus;

	//if ( pTarget && pTarget->GetCanvas() != pCanvas ) { pTarget = NULL; }

	if ( pTarget && !pTarget->Visible() ) { pTarget = NULL; }

	if ( bDown )
	{
		if ( !KeyData.KeyState[ iKey ] )
		{
			KeyData.KeyState[ iKey ] = true;
			KeyData.NextRepeat[ iKey ] = Gwen::Platform::GetTimeInSeconds() + KeyRepeatDelay;
			KeyData.Target = pTarget;

			if ( pTarget )
			{ return pTarget->OnKeyPress( iKey ); }
		}
	}
	else
	{
		if ( KeyData.KeyState[ iKey ] )
		{
			KeyData.KeyState[ iKey ] = false;

			// BUG BUG. This causes shift left arrow in textboxes
			// to not work. What is disabling it here breaking?
			//KeyData.Target = NULL;

			if ( pTarget )
			{ return pTarget->OnKeyRelease( iKey ); }
		}
	}

	return false;
}

bool Gwen::Input::OnCharacter( Gwen::UnicodeChar chr )
{
	//if ( Hidden() ) { return false; }

	if ( !iswprint( chr ) ) { return false; }

	//Handle Accelerators
	if ( Gwen::Input::HandleAccelerator( chr ) )
	{ return true; }

	//Handle characters
	if ( !Gwen::KeyboardFocus ) { return false; }

	//if ( Gwen::KeyboardFocus->GetCanvas() != this ) { return false; }

	if ( !Gwen::KeyboardFocus->Visible() ) { return false; }

	if ( Gwen::Input::IsControlDown() ) { return false; }

	return Gwen::KeyboardFocus->OnChar( chr );
}

bool Gwen::Input::OnMouseWheel( int val )
{
	//if ( Hidden() ) { return false; }

	if ( !Gwen::HoveredControl ) { return false; }

	//if ( Gwen::HoveredControl == this ) { return false; }

	//if ( Gwen::HoveredControl->GetCanvas() != this ) { return false; }

	return Gwen::HoveredControl->OnMouseWheeled( val );
}
