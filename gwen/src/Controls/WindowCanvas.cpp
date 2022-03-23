/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Controls/WindowCanvas.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/Menu.h"
#include "Gwen/DragAndDrop.h"
#include "Gwen/ToolTip.h"
#include "Gwen/Platform.h"

#ifndef GWEN_NO_ANIMATION
#include "Gwen/Anim.h"
#endif

using namespace Gwen;
using namespace Gwen::Controls;

WindowCanvas::WindowCanvas( int x, int y, int w, int h, Gwen::Skin::Base* pSkin, const Gwen::String & strWindowTitle ) : BaseClass( NULL )
{
	m_bHasTitleBar = !Platform::WindowHasTitleBar();
	m_bQuit = false;
	m_bCanMaximize = true;
	m_bIsMaximized = false;
	SetPadding( Padding( 1, 0, 1, 1 ) );
	// Centering the window on the desktop
	{
		int dw, dh;
		Gwen::Platform::GetDesktopSize( dw, dh );

		if ( x < 0 ) { x = ( dw - w ) * 0.5; }

		if ( y < 0 ) { y = ( dh - h ) * 0.5; }
	}
	m_WindowPos  = Gwen::Point( x, y );

	m_pOSWindow = Gwen::Platform::CreatePlatformWindow( x, y, w, h, strWindowTitle, pSkin->GetRender() );
	pSkin->GetRender()->InitializeContext(this);
	pSkin->GetRender()->Init();
	m_pSkinChange = pSkin;
	SetSize( w, h );
	SetMinimumSize(Gwen::Point(std::min(w, 100), std::min(h, 40)));
	
	if (m_bHasTitleBar)
	{
		m_TitleBar = new Gwen::ControlsInternal::Dragger( this );
		m_TitleBar->SetHeight( 24 );
		m_TitleBar->SetPadding( Padding( 0, 0, 0, 0 ) );
		m_TitleBar->SetMargin( Margin( 0, 0, 0, 0 ) );
		m_TitleBar->Dock( Pos::Top );
		m_TitleBar->SetDoMove( false );
		m_TitleBar->onDragged.Add( this, &ThisClass::Dragger_Moved );
		m_TitleBar->onDragStart.Add( this, &ThisClass::Dragger_Start );
		m_TitleBar->onDoubleClickLeft.Add( this, &ThisClass::OnTitleDoubleClicked );
		m_Title = new Gwen::Controls::Label( m_TitleBar );
		m_Title->SetAlignment( Pos::Left | Pos::CenterV );
		m_Title->SetText( strWindowTitle );
		m_Title->Dock( Pos::Fill );
		m_Title->SetPadding( Padding( 8, 0, 0, 0 ) );
		m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive );
		
		// CLOSE
		m_pClose = new Gwen::Controls::WindowCloseButton( m_TitleBar, "Close" );
		m_pClose->Dock( Pos::Right );
		m_pClose->SetMargin( Margin( 0, 0, 4, 0 ) );
		m_pClose->onPress.Add( this, &WindowCanvas::CloseButtonPressed );
		m_pClose->SetTabable( false );
		m_pClose->SetWindow( this );
		
		// MAXIMIZE
		m_pMaximize = new Gwen::Controls::WindowMaximizeButton( m_TitleBar, "Maximize" );
		m_pMaximize->Dock( Pos::Right );
		m_pMaximize->onPress.Add( this, &WindowCanvas::MaximizeButtonPressed );
		m_pMaximize->SetTabable( false );
		m_pMaximize->SetWindow( this );

		// MINIMiZE
		m_pMinimize = new Gwen::Controls::WindowMinimizeButton( m_TitleBar, "Minimize" );
		m_pMinimize->Dock( Pos::Right );
		m_pMinimize->onPress.Add( this, &WindowCanvas::MinimizeButtonPressed );
		m_pMinimize->SetTabable( false );
		m_pMinimize->SetWindow( this );

		// Bottom Right Corner Sizer
		m_Sizer = new Gwen::ControlsInternal::Dragger( this );
		m_Sizer->SetSize( 16, 16 );
		m_Sizer->SetDoMove( false );
		m_Sizer->onDragged.Add( this, &WindowCanvas::Sizer_Moved );
		m_Sizer->onDragStart.Add( this, &WindowCanvas::Dragger_Start );
		m_Sizer->SetCursor( Gwen::CursorType::SizeNWSE );
	}
}

WindowCanvas::~WindowCanvas()
{
	DestroyWindow();
}

void* WindowCanvas::GetWindow()
{
	return m_pOSWindow;
}

void WindowCanvas::Layout( Skin::Base* skin )
{
	if (m_bHasTitleBar)
	{
		m_Sizer->BringToFront();
		m_Sizer->SetSize(16, 16);
		m_Sizer->Position( Pos::Right | Pos::Bottom );
	}
	BaseClass::Layout( skin );
}

void WindowCanvas::DoThink()
{
	Platform::MessagePump( m_pOSWindow, this );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::RenderCanvas()
{
	//
	// If there isn't anything going on we sleep the thread for a few ms
	// This gives some cpu time back to the os. If you're using a rendering
	// method that needs continual updates, just call canvas->redraw every frame.
	//
	if ( !NeedsRedraw() )
	{
		Platform::Sleep( 10 );
		return;
	}

	m_bNeedsRedraw = false;
	Gwen::Renderer::Base* render = m_Skin->GetRender();

	if ( render->BeginContext( this ) )
	{
		render->Begin();
		RecurseLayout( m_Skin );
		render->SetClipRegion( GetRenderBounds() );
		render->SetRenderOffset( Gwen::Point( X() * -1, Y() * -1 ) );
		render->SetScale( Scale() );

		if ( m_bDrawBackground )
		{
			render->SetDrawColor( m_BackgroundColor );
			render->DrawFilledRect( GetRenderBounds() );
		}

		DoRender( m_Skin );
		DragAndDrop::RenderOverlay( this, m_Skin );
		ToolTip::RenderToolTip( static_cast<Canvas*>(this), m_Skin );
		render->End();
	}

	render->EndContext( this );
	render->PresentContext( this );
}

void WindowCanvas::Render( Skin::Base* skin )
{
	bool bHasFocus = IsOnTop();

	if (m_bHasTitleBar)
	{
		if ( bHasFocus )
		{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleActive ); }
		else
		{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive ); }

		skin->DrawWindow( this, m_TitleBar->Bottom(), bHasFocus );
	}
	else
	{
		skin->DrawWindow( this, 0, bHasFocus );
	}
}


void WindowCanvas::DestroyWindow()
{
	if ( m_pOSWindow )
	{
		GetSkin()->GetRender()->ShutdownContext( this );
		Gwen::Platform::DestroyPlatformWindow( m_pOSWindow );
		m_pOSWindow = NULL;
	}
}

bool WindowCanvas::InputQuit()
{
	m_bQuit = true;
	return true;
}

Skin::Base* WindowCanvas::GetSkin( void )
{
	if ( m_pSkinChange )
	{
		SetSkin( m_pSkinChange );
		m_pSkinChange = NULL;
	}

	return BaseClass::GetSkin();
}


void WindowCanvas::Dragger_Start()
{
	Gwen::Platform::GetCursorPos( m_HoldPos );
	m_HoldPos.x -= m_WindowPos.x;
	m_HoldPos.y -= m_WindowPos.y;
}

void WindowCanvas::Dragger_Moved()
{
	Gwen::Point p;
	Gwen::Platform::GetCursorPos( p );

	//
	// Dragged out of maximized
	//
	if ( m_bIsMaximized )
	{
		float fOldWidth = Width();
		SetMaximize( false );
		// Change the hold pos to be the same distance across the titlebar of the resized window
		m_HoldPos.x = ( ( float ) m_HoldPos.x ) * ( ( float ) Width() / fOldWidth );
		m_HoldPos.y = 10;
	}

	SetPos( p.x - m_HoldPos.x, p.y - m_HoldPos.y );
}

void WindowCanvas::SetPos( int x, int y )
{
	int w, h;
	Gwen::Platform::GetDesktopSize( w, h );
	y = Gwen::Clamp( y, 0, h );
	m_WindowPos.x = x;
	m_WindowPos.y = y;
	Gwen::PointF scaling = GetDPIScaling();
	Gwen::Platform::SetBoundsPlatformWindow( m_pOSWindow, x, y, Width()*scaling.x, Height()*scaling.y);
}

void WindowCanvas::CloseButtonPressed()
{
	InputQuit();
}

bool WindowCanvas::IsOnTop()
{
	return Gwen::Platform::HasFocusPlatformWindow( m_pOSWindow );
}


void WindowCanvas::Sizer_Moved()
{
	Gwen::Point p;
	Gwen::Platform::GetCursorPos( p );
	int w = ( p.x ) - m_WindowPos.x;
	int h = ( p.y ) - m_WindowPos.y;
	w = Clamp( w, 100, 9999 );
	h = Clamp( h, 100, 9999 );
	Gwen::PointF scaling = GetDPIScaling();
	Gwen::Platform::SetBoundsPlatformWindow( m_pOSWindow, m_WindowPos.x, m_WindowPos.y, w*scaling.x, h*scaling.y);
	GetSkin()->GetRender()->ResizedContext( this, w*scaling.x, h*scaling.y);
	this->SetSize( w, h );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::OnTitleDoubleClicked()
{
	if ( !CanMaximize() ) { return; }

	SetMaximize( !m_bIsMaximized );
}

void WindowCanvas::SetMaximize( bool b )
{
	m_bIsMaximized = b;
	if (m_bHasTitleBar) { m_pMaximize->SetMaximized( m_bIsMaximized ); }
	Gwen::Point pSize, pPos;
	Gwen::PointF scaling = GetDPIScaling();
	Gwen::Platform::SetWindowMaximized( m_pOSWindow, m_bIsMaximized, pPos, pSize );
	SetSize( pSize.x/scaling.x, pSize.y/scaling.y );
	m_WindowPos = pPos;
	GetSkin()->GetRender()->ResizedContext( this, pSize.x, pSize.y );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::MaximizeButtonPressed()
{
	if ( !CanMaximize() ) { return; }

	SetMaximize( !m_bIsMaximized );
}

void WindowCanvas::MinimizeButtonPressed()
{
	Gwen::Platform::SetWindowMinimized( m_pOSWindow, true );
}

void WindowCanvas::Minimize()
{
	Gwen::Platform::SetWindowMinimized(m_pOSWindow, true);
}

void WindowCanvas::SetCanMaximize( bool b )
{
	if ( m_bCanMaximize == b ) { return; }

	m_bCanMaximize = b;
	if (m_bHasTitleBar) { m_pMaximize->SetDisabled( !b ); }
}

void WindowCanvas::SetMinimumSize( const Gwen::Point & minSize )
{
	m_MinimumSize = minSize;
	
	Gwen::Platform::SetWindowMinimumSize(m_pOSWindow, minSize.x, minSize.y);
}
