/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/Macros.h"
#include "Gwen/Platform.h"

#ifdef GWEN_ALLEGRO_PLATFORM

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#include "Gwen/Input/Allegro.h"


static Gwen::Input::Allegro     g_GwenInput;
static ALLEGRO_EVENT_QUEUE    * g_event_queue = NULL;
static ALLEGRO_DISPLAY        * g_display = NULL;
static Gwen::UnicodeString      gs_ClipboardEmulator;

static const ALLEGRO_SYSTEM_MOUSE_CURSOR g_CursorConversion[] =
{
	ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW,      // IDC_ARROW
	ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT,       // IDC_IBEAM
	ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N,   // IDC_SIZENS
	ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E,   // IDC_SIZEWE
	ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N,   // IDC_SIZENWSE
	ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E,   // IDC_SIZENESW
	ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE,       // IDC_SIZEALL
	ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE,// IDC_NO
	ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY,       // IDC_WAIT
	ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK        // IDC_HAND
};


void Gwen::Platform::Sleep( unsigned int iMS )
{
	al_rest(iMS * 0.001);
}

void Gwen::Platform::SetCursor( unsigned char iCursor )
{
	al_set_system_mouse_cursor(g_display, g_CursorConversion[iCursor]);
}

Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
	return gs_ClipboardEmulator;
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString& str )
{
	gs_ClipboardEmulator = str;
	return true;
}

float Gwen::Platform::GetTimeInSeconds()
{
	return al_get_time();
}

bool Gwen::Platform::FileOpen( const String& Name, const String& StartPath,
							   const String& Extension, Gwen::Event::Handler* pHandler,
							   Event::Handler::FunctionWithInformation fnCallback )
{
	ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(StartPath.c_str(),
																Name.c_str(),
																"*.*", // Extension.c_str(),
																ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);

	if ( al_show_native_file_dialog(g_display, chooser) )
	{
		if ( al_get_native_file_dialog_count(chooser) != 0)
		{
			if ( pHandler && fnCallback )
			{
				Gwen::Event::Information info;
				info.Control		= NULL;
				info.ControlCaller	= NULL;
				info.String			= al_get_native_file_dialog_path(chooser, 0);

				(pHandler->*fnCallback)( info );
			}
		}
	}

	al_destroy_native_file_dialog(chooser);

	return true;
}

bool Gwen::Platform::FileSave( const String& Name, const String& StartPath,
							   const String& Extension, Gwen::Event::Handler* pHandler,
							   Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(StartPath.c_str(),
																Name.c_str(),
																"*.*", // Extension.c_str(),
																ALLEGRO_FILECHOOSER_SAVE);

	if ( al_show_native_file_dialog(g_display, chooser) )
	{
		if ( al_get_native_file_dialog_count(chooser) != 0)
		{
			if ( pHandler && fnCallback )
			{
				Gwen::Event::Information info;
				info.Control		= NULL;
				info.ControlCaller	= NULL;
				info.String			= al_get_native_file_dialog_path(chooser, 0);

				(pHandler->*fnCallback)( info );
			}
		}
	}

	al_destroy_native_file_dialog(chooser);

	return true;
}

bool Gwen::Platform::FolderOpen( const String& Name, const String& StartPath,
								 Gwen::Event::Handler* pHandler,
								 Event::Handler::FunctionWithInformation fnCallback )
{
	ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(StartPath.c_str(),
																Name.c_str(),
																"*.*", // Extension.c_str(),
																ALLEGRO_FILECHOOSER_FOLDER);

	if ( al_show_native_file_dialog(g_display, chooser) )
	{
		if ( al_get_native_file_dialog_count(chooser) != 0)
		{
			if ( pHandler && fnCallback )
			{
				Gwen::Event::Information info;
				info.Control		= NULL;
				info.ControlCaller	= NULL;
				info.String			= al_get_native_file_dialog_path(chooser, 0);

				(pHandler->*fnCallback)( info );
			}
		}
	}

	al_destroy_native_file_dialog(chooser);

	return true;
}

void* Gwen::Platform::CreatePlatformWindow( int x, int y, int w, int h, const Gwen::String& strWindowTitle )
{
	if ( !al_init() ) return NULL;

	al_set_new_window_position( x, y );
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);
	ALLEGRO_DISPLAY *display = al_create_display( w, h );
	if ( !display) return NULL;
	g_display = display;

	g_event_queue = al_create_event_queue();
	if ( !g_event_queue ) return NULL;

	al_init_image_addon();
	al_init_font_addon();
	al_init_primitives_addon();
	al_init_ttf_addon();

	al_install_mouse();
	al_install_keyboard();

	al_register_event_source( g_event_queue, al_get_display_event_source(display) );
	al_register_event_source( g_event_queue, al_get_mouse_event_source() );
	al_register_event_source( g_event_queue, al_get_keyboard_event_source() );

	return display;
}

void Gwen::Platform::DestroyPlatformWindow( void* pPtr )
{
	ALLEGRO_DISPLAY *display = (ALLEGRO_DISPLAY*)pPtr;
	al_destroy_display( display );
	al_destroy_event_queue( g_event_queue );
	g_event_queue = NULL;
}

void Gwen::Platform::MessagePump( void* pWindow, Gwen::Controls::Canvas* ptarget )
{
	static bool firstCall = true;
	if (firstCall)
	{
		firstCall = false;
		g_GwenInput.Initialize( ptarget );
	}

	ALLEGRO_EVENT ev;
	while ( al_get_next_event( g_event_queue, &ev) )
	{
		g_GwenInput.ProcessMessage( ev );
	}
}

void Gwen::Platform::SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h )
{

}

void Gwen::Platform::SetWindowMaximized( void* pPtr, bool bMax, Gwen::Point& pNewPos, Gwen::Point& pNewSize )
{
	ALLEGRO_DISPLAY *display = (ALLEGRO_DISPLAY*)pPtr;
	al_set_window_position(display, pNewPos.x, pNewPos.y);
	al_resize_display(display, pNewSize.x, pNewSize.y);
}

void Gwen::Platform::SetWindowMinimized( void* pPtr, bool bMinimized )
{
}

bool Gwen::Platform::HasFocusPlatformWindow( void* pPtr )
{
	return true;
}

void Gwen::Platform::GetDesktopSize( int& w, int &h )
{
	w = 1024;
	h = 768;
}

void Gwen::Platform::GetCursorPos( Gwen::Point &po )
{

}

#endif // GWEN_ALLEGRO_PLATFORM
