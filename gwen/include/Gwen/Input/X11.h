/*
	GWEN
	Copyright (c) 2011 Facepunch Studios and Matthew Bries
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
					m_MouseX = 0;
					m_MouseY = 0;
				}

				bool ProcessMessage( XEvent& event )
				{
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
								int x = event.xmotion.x_root;
								int y = event.xmotion.y_root;
								
								int dx = x - m_MouseX;
								int dy = y - m_MouseY;
								m_MouseX = x;
								m_MouseY = y;
								
								return Gwen::Input::OnMouseMoved( x, y, dx, dy, (void*)event.xmotion.window);
							}
                        case KeyRelease:
                            press = false;
						case KeyPress:
							{	
								int len = XLookupString(&event.xkey, text, 255, &key, 0);
								if (Gwen::Input::IsControlDown())
								{
									// handle control messing with the lookup string
									text[0] = XLookupKeysym(&event.xkey, 0);
								}
								if (len == 1 && (text[0] >= 32 && text[0] <= 126))
								{
									if (text[0] == ' ') { Gwen::Input::OnKeyEvent( Gwen::Key::Space, press); }

                                    if (!press) { return false; }
                                    
									Gwen::UnicodeChar chr = ( Gwen::UnicodeChar ) text[0];
									return Gwen::Input::OnCharacter( chr );
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
									return Gwen::Input::OnKeyEvent( iKey, press);
								}
							}

                        case ButtonRelease:
                            {
                                press = false;
                            }
                        case ButtonPress:
                            {
								if (event.xbutton.button == 1)
									return Gwen::Input::OnMouseButton(0, press);
								if (event.xbutton.button == 2)
									return Gwen::Input::OnMouseButton(2, press);
								if (event.xbutton.button == 3)
									return Gwen::Input::OnMouseButton(1, press);
                                // Scrolling is mapped to buttons....
								if (event.xbutton.button == 4)
									return Gwen::Input::OnMouseWheel(20);
								if (event.xbutton.button == 5)
									return Gwen::Input::OnMouseWheel(-20);
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
