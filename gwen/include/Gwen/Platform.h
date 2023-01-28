/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_PLATFORM_H
#define GWEN_PLATFORM_H

#include "Gwen/Structures.h"
#include "Gwen/Events.h"
#include "Gwen/BaseRender.h"
#include "Gwen/Controls/WindowCanvas.h"

namespace Gwen
{
	namespace Platform
	{
		//
		// Do nothing for this many milliseconds
		//
		GWEN_EXPORT void Sleep( unsigned int iMS );

		//
		// Set the system cursor to iCursor
		// Cursors are defined in Structures.h
		//
		GWEN_EXPORT void SetCursor( unsigned char iCursor );

		//
		//
		//
		GWEN_EXPORT void GetCursorPos( Gwen::Point & p );
		GWEN_EXPORT void GetDesktopSize( int & w, int & h );

		//
		// Used by copy/paste
		//
		GWEN_EXPORT UnicodeString GetClipboardText();
		GWEN_EXPORT bool SetClipboardText( const UnicodeString & str );

		//
		// Needed for things like double click
		//
		GWEN_EXPORT float GetTimeInSeconds();

		//
		// System Dialogs ( Can return false if unhandled )
		//
		GWEN_EXPORT bool FileOpen( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback );
		GWEN_EXPORT bool FileSave( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback );
		GWEN_EXPORT bool FolderOpen( const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback );


		//
		// Window Creation
		//
		GWEN_EXPORT void* CreatePlatformWindow( int x, int y, int w, int h, const Gwen::String & strWindowTitle, Gwen::Renderer::Base* renderer, bool is_menu);
		GWEN_EXPORT void DestroyPlatformWindow( void* pPtr );
		GWEN_EXPORT void WaitForEvent( int timeout_ms );
		GWEN_EXPORT void InterruptWait();
		GWEN_EXPORT void SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h );
		GWEN_EXPORT void MessagePump( void* pWindow, Gwen::Controls::WindowCanvas* ptarget );
		GWEN_EXPORT bool HasFocusPlatformWindow( void* pPtr );
		GWEN_EXPORT void SetWindowMaximized( void* pPtr, bool bMaximized, Gwen::Point & pNewPos, Gwen::Point & pNewSize );
		GWEN_EXPORT void SetWindowMinimized( void* pPtr, bool bMinimized );
		GWEN_EXPORT void SetWindowMinimumSize( void* pPtr, int min_width, int min_height);
		GWEN_EXPORT bool WindowHasTitleBar();
		GWEN_EXPORT bool IsWindowMaximized( void* pPtr);
		GWEN_EXPORT void SetWindowTitle(void* pPtr, const Gwen::String & strWindowTitle);
	}

}
#endif
