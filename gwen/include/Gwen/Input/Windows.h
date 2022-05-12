/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/
#ifndef GWEN_INPUT_WINDOWS_H
#define GWEN_INPUT_WINDOWS_H

#include "Gwen/InputHandler.h"
#include "Gwen/Gwen.h"
#include "Gwen/Controls/Canvas.h"

#include <windows.h>

namespace Gwen
{
	namespace Input
	{
		class Windows
		{
			public:

				Windows()
				{
					m_MouseX = 0;
					m_MouseY = 0;
				}

				bool ProcessMessage( MSG msg )
				{
					switch ( msg.message )
					{
							//case WM_NCLBUTTONDOWN:
						case WM_SYSCOMMAND:
							{
								if ( msg.message == WM_SYSCOMMAND && msg.wParam != SC_CLOSE )
								{ return false; }

								//return m_Canvas->InputQuit();
							}

						case WM_MOUSEMOVE:
							{
								int x = ( signed short ) LOWORD( msg.lParam );
								int y = ( signed short ) HIWORD( msg.lParam );

								RECT r;
								GetWindowRect(msg.hwnd, &r);

								int new_mouse_x = x + r.left;
								int new_mouse_y = y + r.top;

								int dx = new_mouse_x - m_MouseX;
								int dy = new_mouse_y - m_MouseY;

								m_MouseX = new_mouse_x;
								m_MouseY = new_mouse_y;
								return Gwen::Input::OnMouseMoved(m_MouseX, m_MouseY, dx, dy, (void*)msg.hwnd );
							}

						case WM_CHAR:
							{
								Gwen::UnicodeChar chr = ( Gwen::UnicodeChar ) msg.wParam;
								return Gwen::Input::OnCharacter( chr );
							}

#ifdef WM_MOUSEWHEEL

						case WM_MOUSEWHEEL:
							{
								return Gwen::Input::OnMouseWheel(20 * ((short)HIWORD(msg.wParam)) / WHEEL_DELTA);
							}

#endif

						case WM_LBUTTONDOWN:
							{
								bool ret = Gwen::Input::OnMouseButton(0, true);
								if (ret)
									SetCapture( msg.hwnd );
								return ret;
							}
						
						case WM_LBUTTONUP:
							{
								bool ret = Gwen::Input::OnMouseButton(0, false);
								if (ret)
									ReleaseCapture();
								return ret;
							}
						
						case WM_RBUTTONDOWN:
							{
								bool ret = Gwen::Input::OnMouseButton(1, true);
								if (ret)
								   SetCapture(msg.hwnd);
								return ret;
							}

						case WM_RBUTTONUP:
							{
								bool ret = Gwen::Input::OnMouseButton(1, false);
								if (ret)
									 ReleaseCapture();
								return ret;
							}

						case WM_MBUTTONDOWN:
							{
								bool ret = Gwen::Input::OnMouseButton(2, true);
								if (ret)
								   SetCapture(msg.hwnd);
								return ret;
							}

						case WM_MBUTTONUP:
							{
								bool ret = Gwen::Input::OnMouseButton(2, false);
								if (ret)
								 ReleaseCapture();
								return ret;
							}

						case WM_LBUTTONDBLCLK:
						case WM_RBUTTONDBLCLK:
						case WM_MBUTTONDBLCLK:
							{
								// Filter out those events from the application
								return true;
							}

						case WM_KEYDOWN:
						case WM_KEYUP:
							{
								bool bDown = msg.message == WM_KEYDOWN;
								int iKey = -1;

								// These aren't sent by WM_CHAR when CTRL is down - but we need
								// them internally for copy and paste etc..
								if ( bDown && GetKeyState( VK_CONTROL ) & 0x80 && msg.wParam >= 'A' && msg.wParam <= 'Z' )
								{
									Gwen::UnicodeChar chr = ( Gwen::UnicodeChar ) msg.wParam;
									return Gwen::Input::OnCharacter( chr );
								}

								if ( msg.wParam == VK_SHIFT ) { iKey = Gwen::Key::Shift; }
								else if ( msg.wParam == VK_RETURN ) { iKey = Gwen::Key::Return; }
								else if ( msg.wParam == VK_BACK ) { iKey = Gwen::Key::Backspace; }
								else if ( msg.wParam == VK_DELETE ) { iKey = Gwen::Key::Delete; }
								else if ( msg.wParam == VK_LEFT ) { iKey = Gwen::Key::Left; }
								else if ( msg.wParam == VK_RIGHT ) { iKey = Gwen::Key::Right; }
								else if ( msg.wParam == VK_TAB ) { iKey = Gwen::Key::Tab; }
								else if ( msg.wParam == VK_SPACE ) { iKey = Gwen::Key::Space; }
								else if ( msg.wParam == VK_HOME ) { iKey = Gwen::Key::Home; }
								else if ( msg.wParam == VK_END ) { iKey = Gwen::Key::End; }
								else if ( msg.wParam == VK_CONTROL ) { iKey = Gwen::Key::Control; }
								else if ( msg.wParam == VK_SPACE ) { iKey = Gwen::Key::Space; }
								else if ( msg.wParam == VK_UP ) { iKey = Gwen::Key::Up; }
								else if ( msg.wParam == VK_DOWN ) { iKey = Gwen::Key::Down; }
								else if (msg.wParam >= VK_F1 && msg.wParam <= VK_F24) { iKey = Gwen::Key::F1 + msg.wParam - VK_F1; }
								else if (msg.wParam == VK_PRIOR) { iKey = Gwen::Key::PageUp; }
								else if (msg.wParam == VK_NEXT) { iKey = Gwen::Key::PageDown; }
								
								if ( iKey != -1 )
								{
									return Gwen::Input::OnKeyEvent( iKey, bDown );
								}

								break;
							}

						default:
							{
								break;
							}
					}

					return false;
				}

			protected:

				int m_MouseX;
				int m_MouseY;

		};
	}
}
#endif
