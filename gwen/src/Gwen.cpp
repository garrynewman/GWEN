/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include <stdio.h>
#include <stdarg.h>


namespace Gwen
{
	// Globals
	GWEN_EXPORT Controls::Base* HoveredControl = NULL;
	GWEN_EXPORT Controls::Base* KeyboardFocus = NULL;
	GWEN_EXPORT Controls::Base* MouseFocus = NULL;

	namespace Debug
	{
		void Msg( const char* str, ... )
		{
			char strOut[1024];
			va_list s;
			va_start( s, str );
			vsnprintf( strOut, sizeof( strOut ), str, s );
			va_end( s );
			GwenUtil_OutputDebugCharString( strOut );
		}
#ifdef UNICODE
		void Msg( const wchar_t* str, ... )
		{
			wchar_t strOut[1024];
			va_list s;
			va_start( s, str );
			vswprintf( strOut, sizeof( strOut ), str, s );
			va_end( s );
			GwenUtil_OutputDebugWideString( strOut );
		}
#endif
		void AssertCheck( bool b, const char* strMsg )
		{
			if ( b ) { return; }

			Msg( "Assert: %s\n", strMsg );
#ifdef _WIN32
			MessageBoxA( NULL, strMsg, "Assert", MB_ICONEXCLAMATION | MB_OK );
			__debugbreak();
#endif
		}
	}

}
