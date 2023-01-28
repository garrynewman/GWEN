/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_INPUTHANDLER_H
#define GWEN_INPUTHANDLER_H

#include <queue>
#include "Gwen/Gwen.h"

namespace Gwen
{
	namespace Controls
	{
		class Base;
	}

	namespace Key
	{
		const unsigned char Invalid = 0;
		const unsigned char Return = 1;
		const unsigned char Backspace = 2;
		const unsigned char Delete = 3;
		const unsigned char Left = 4;
		const unsigned char Right = 5;
		const unsigned char Shift = 6;
		const unsigned char Tab = 7;
		const unsigned char Space = 8;
		const unsigned char Home = 9;
		const unsigned char End = 10;
		const unsigned char Control = 11;
		const unsigned char Up = 12;
		const unsigned char Down = 13;
		const unsigned char Escape = 14;
		const unsigned char Alt = 15;
		const unsigned char F1 = 16;
		const unsigned char F2 = 17;
		const unsigned char F3 = 18;
		const unsigned char F4 = 19;
		const unsigned char F5 = 20;
		const unsigned char F10 = 25;
		const unsigned char F15 = 30;
		const unsigned char F20 = 35;
		const unsigned char F24 = 39;

		const unsigned char PageUp = 40;
		const unsigned char PageDown = 41;

		const unsigned char Count = 42;
	}

	namespace Input
	{
		namespace Message
		{
			enum
			{
				Copy,
				Paste,
				Cut,
				Undo,
				Redo,
				SelectAll
			};
		};

		// Register canvases that events should be forwarded to		
		void GWEN_EXPORT RegisterCanvas( Controls::Canvas* pCanvas );
		void GWEN_EXPORT RemoveCanvas( Controls::Canvas* pCanvas );

		// For use in panels
		bool GWEN_EXPORT IsKeyDown( int iKey );
		bool GWEN_EXPORT IsLeftMouseDown();
		bool GWEN_EXPORT IsRightMouseDown();
		Gwen::Point GWEN_EXPORT GetMousePosition();

		void UpdateHovered();

		inline bool IsShiftDown() { return IsKeyDown( Gwen::Key::Shift ); }
		inline bool IsControlDown() { return IsKeyDown( Gwen::Key::Control ); }

		// Does copy, paste etc
		bool GWEN_EXPORT DoSpecialKeys( Controls::Base* pCanvas, Gwen::UnicodeChar chr );
		bool GWEN_EXPORT HandleAccelerator( Gwen::UnicodeChar chr );

		// Send input to canvas for study
		bool GWEN_EXPORT OnMouseMoved( int x, int y, int deltaX, int deltaY, void* platform_window );
		bool GWEN_EXPORT OnMouseButton( int iButton, bool bDown );
		bool GWEN_EXPORT OnMouseWheel( int val );
		bool GWEN_EXPORT OnKeyEvent( int iKey, bool bDown );
		bool GWEN_EXPORT OnCharacter( Gwen::UnicodeChar chr );
		void GWEN_EXPORT OnThink();
	};
}
#endif
