/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/
#ifndef GWEN_INPUT_X11_H
#define GWEN_INPUT_X11_H

#include "Gwen/InputHandler.h"
#include "Gwen/Gwen.h"
#include "Gwen/Controls/Canvas.h"

namespace Gwen
{
	namespace Input
	{
		class X11
		{
			public:

				X11()
				{
					m_Canvas = NULL;
					m_MouseX = 0;
					m_MouseY = 0;
				}

				void Initialize( Gwen::Controls::Canvas* c )
				{
					m_Canvas = c;
				}

				bool ProcessMessage( XEvent& event )
				{
					if ( !m_Canvas ) { return false; }

                    bool press = true;
                    KeySym key;
                    char text[255];
					switch ( event.type )
					{
							//case WM_NCLBUTTONDOWN:
						/*case WM_SYSCOMMAND:
							{
								if ( msg.message == WM_SYSCOMMAND && msg.wParam != SC_CLOSE )
								{ return false; }

								return m_Canvas->InputQuit();
							}*/

						case MotionNotify:
							{
                                int x = event.xmotion.x;
                                int y = event.xmotion.y;
								//int x = ( signed short ) LOWORD( msg.lParam );
								//int y = ( signed short ) HIWORD( msg.lParam );
								int dx = x - m_MouseX;
								int dy = y - m_MouseY;
								m_MouseX = x;
								m_MouseY = y;
								//Gwen::PointF scale = m_Canvas->GetSkin()->GetRender()->GetDPIScaling();*/
								return m_Canvas->InputMouseMoved( x, y, dx, dy);
							}
                        case KeyRelease:
                            press = false;
						case KeyPress:
							{
                                int len = XLookupString(&event.xkey, text, 255, &key, 0);
                                if ((key >= 33 && key <= 126) && len == 1)
                                {
                                    if (!press) { return false; }
								    Gwen::UnicodeChar chr = ( Gwen::UnicodeChar ) key;
								    return m_Canvas->InputCharacter( chr );
                                }

                                int iKey = -1;
                                if (key == XK_BackSpace) { iKey = Gwen::Key::Backspace; }
                                else if (key == XK_Tab) { iKey = Gwen::Key::Tab; }
                                else if (key == XK_Delete) { iKey = Gwen::Key::Delete; }
                                else if (key == XK_Return) { iKey = Gwen::Key::Return; }
                                else if (key == XK_Left) { iKey = Gwen::Key::Left; }
                                else if (key == XK_Up) { iKey = Gwen::Key::Up; }
                                else if (key == XK_Down) { iKey = Gwen::Key::Down; }
                                else if (key == XK_Right) { iKey = Gwen::Key::Right; }
                                else if (key == XK_End) { iKey = Gwen::Key::End; }
                                else if (key == XK_Home) { iKey = Gwen::Key::Home; }
                                else if (key == XK_Prior) { iKey = Gwen::Key::PageUp; }
                                else if (key == XK_Next) { iKey = Gwen::Key::PageDown; }
                                else if (key == XK_Shift_L || key == XK_Shift_R) { iKey = Gwen::Key::Shift; }
                                else if (key == XK_Control_L || key == XK_Control_R) { iKey = Gwen::Key::Control; }
								/*else if (msg.wParam >= VK_F1 && msg.wParam <= VK_F24) { iKey = Gwen::Key::F1 + msg.wParam - VK_F1; }*/
								
								if ( iKey != -1 )
								{
								    return m_Canvas->InputKey( iKey, press );
								}
							}

                        case ButtonRelease:
                            {
                                press = false;                                
                            }
                        case ButtonPress:
                            {
                                if (event.xbutton.button == 1)
                                    return m_Canvas->InputMouseButton(0, press);
                                if (event.xbutton.button == 2)
                                    return m_Canvas->InputMouseButton(2, press);
                                if (event.xbutton.button == 3)
                                    return m_Canvas->InputMouseButton(1, press);
                                // Scrolling is mapped to buttons....
                                if (event.xbutton.button == 4)
                                    return m_Canvas->InputMouseWheel(20);
                                if (event.xbutton.button == 5)
                                    return m_Canvas->InputMouseWheel(-20);
                            }


/*

						case WM_LBUTTONDOWN:
							{
								bool ret = m_Canvas->InputMouseButton(0, true);
								if (ret)
									SetCapture( msg.hwnd );
								return ret;
							}
						
						case WM_LBUTTONUP:
							{
								bool ret = m_Canvas->InputMouseButton(0, false);
								if (ret)
									ReleaseCapture();
								return ret;
							}
						
						case WM_RBUTTONDOWN:
							{
								bool ret = m_Canvas->InputMouseButton(1, true);
								if (ret)
								   SetCapture(msg.hwnd);
								return ret;
							}

						case WM_RBUTTONUP:
							{
								bool ret = m_Canvas->InputMouseButton(1, false);
								if (ret)
									 ReleaseCapture();
								return ret;
							}

						case WM_MBUTTONDOWN:
							{
								bool ret = m_Canvas->InputMouseButton(2, true);
								if (ret)
								   SetCapture(msg.hwnd);
								return ret;
							}

						case WM_MBUTTONUP:
							{
								bool ret = m_Canvas->InputMouseButton(2, false);
								if (ret)
								 ReleaseCapture();
								return ret;
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
									return m_Canvas->InputCharacter( chr );
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
									return m_Canvas->InputKey( iKey, bDown );
								}

								break;
							}*/

						default:
							{
								break;
							}
					}

					return false;
				}

			protected:

				Gwen::Controls::Canvas*	m_Canvas;
				int m_MouseX;
				int m_MouseY;

		};
	}
}
#endif
