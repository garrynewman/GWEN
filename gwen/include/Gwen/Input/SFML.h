/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_INPUT_SFML_H
#define GWEN_INPUT_SFML_H

#include "Gwen/InputHandler.h"
#include "Gwen/Gwen.h"
#include "Gwen/Controls/Canvas.h"

#include <SFML/Window/Event.hpp>

namespace Gwen
{
	namespace Input
	{
		class SFML
		{
			public:

				SFML()
				{
					m_Canvas = NULL;
					m_MouseX = 0;
					m_MouseY = 0;
				}

				void Initialize( Gwen::Controls::Canvas* c )
				{
					m_Canvas = c;
				}

				unsigned char TranslateKeyCode( int iKeyCode )
				{
					switch ( iKeyCode )
					{
#if SFML_VERSION_MAJOR == 2

						case sf::Keyboard::BackSpace:
							return Gwen::Key::Backspace;

						case sf::Keyboard::Return:
							return Gwen::Key::Return;

						case sf::Keyboard::Escape:
							return Gwen::Key::Escape;

						case sf::Keyboard::Tab:
							return Gwen::Key::Tab;

						case sf::Keyboard::Space:
							return Gwen::Key::Space;

						case sf::Keyboard::Up:
							return Gwen::Key::Up;

						case sf::Keyboard::Down:
							return Gwen::Key::Down;

						case sf::Keyboard::Left:
							return Gwen::Key::Left;

						case sf::Keyboard::Right:
							return Gwen::Key::Right;

						case sf::Keyboard::Home:
							return Gwen::Key::Home;

						case sf::Keyboard::End:
							return Gwen::Key::End;

						case sf::Keyboard::Delete:
							return Gwen::Key::Delete;

						case sf::Keyboard::LControl:
							return Gwen::Key::Control;

						case sf::Keyboard::LAlt:
							return Gwen::Key::Alt;

						case sf::Keyboard::LShift:
							return Gwen::Key::Shift;

						case sf::Keyboard::RControl:
							return Gwen::Key::Control;

						case sf::Keyboard::RAlt:
							return Gwen::Key::Alt;

						case sf::Keyboard::RShift:
							return Gwen::Key::Shift;
#else

						case sf::Key::Back:
							return Gwen::Key::Backspace;

						case sf::Key::Return:
							return Gwen::Key::Return;

						case sf::Key::Escape:
							return Gwen::Key::Escape;

						case sf::Key::Tab:
							return Gwen::Key::Tab;

						case sf::Key::Space:
							return Gwen::Key::Space;

						case sf::Key::Up:
							return Gwen::Key::Up;

						case sf::Key::Down:
							return Gwen::Key::Down;

						case sf::Key::Left:
							return Gwen::Key::Left;

						case sf::Key::Right:
							return Gwen::Key::Right;

						case sf::Key::Home:
							return Gwen::Key::Home;

						case sf::Key::End:
							return Gwen::Key::End;

						case sf::Key::Delete:
							return Gwen::Key::Delete;

						case sf::Key::LControl:
							return Gwen::Key::Control;

						case sf::Key::LAlt:
							return Gwen::Key::Alt;

						case sf::Key::LShift:
							return Gwen::Key::Shift;

						case sf::Key::RControl:
							return Gwen::Key::Control;

						case sf::Key::RAlt:
							return Gwen::Key::Alt;

						case sf::Key::RShift:
							return Gwen::Key::Shift;
#endif
					}

					return Gwen::Key::Invalid;
				}

				bool ProcessMessage( sf::Event & event )
				{
					if ( !m_Canvas ) { return false; }

#if SFML_VERSION_MAJOR == 2

					switch ( event.type )
#else
					switch ( event.Type )
#endif
					{
						case sf::Event::MouseMoved:
							{
#if SFML_VERSION_MAJOR == 2
								int dx = event.mouseMove.x - m_MouseX;
								int dy = event.mouseMove.y - m_MouseY;
								m_MouseX = event.mouseMove.x;
								m_MouseY = event.mouseMove.y;
#else
								int dx = event.MouseMove.X - m_MouseX;
								int dy = event.MouseMove.Y - m_MouseY;
								m_MouseX = event.MouseMove.X;
								m_MouseY = event.MouseMove.Y;
#endif
								return m_Canvas->InputMouseMoved( m_MouseX, m_MouseY, dx, dy );
							}

						case sf::Event::MouseButtonPressed:
						case sf::Event::MouseButtonReleased:
							{
#if SFML_VERSION_MAJOR == 2
								return m_Canvas->InputMouseButton( event.mouseButton.button, event.type == sf::Event::MouseButtonPressed );
#else
								return m_Canvas->InputMouseButton( event.MouseButton.Button, event.Type == sf::Event::MouseButtonPressed );
#endif
							}

						case sf::Event::MouseWheelMoved:
							{
#if SFML_VERSION_MAJOR == 2
								return m_Canvas->InputMouseWheel( event.mouseWheel.delta * 60 );
#else
								return m_Canvas->InputMouseWheel( event.MouseWheel.Delta * 60 );
#endif
							}

						case sf::Event::TextEntered:
							{
#if SFML_VERSION_MAJOR == 2
								return m_Canvas->InputCharacter( event.text.unicode );
#else
								return m_Canvas->InputCharacter( event.Text.Unicode );
#endif
							}

						case sf::Event::KeyPressed:
						case sf::Event::KeyReleased:
							{
#if SFML_VERSION_MAJOR == 2
								bool bPressed = ( event.type == sf::Event::KeyPressed );
								char keyCode = event.key.code + 97; // adding 97 here turns the sf::Keyboard::Key into a lowercase ASCII character
								bool control = event.key.control;
#else
								bool bPressed = ( event.Type == sf::Event::KeyPressed );
								char keyCode = event.Key.Code;
								bool control = event.Key.Control;
#endif

								if ( control && bPressed && keyCode >= 'a' && keyCode <= 'z' )
								{
									return m_Canvas->InputCharacter( keyCode );
								}

#if SFML_VERSION_MAJOR == 2
								unsigned char iKey = TranslateKeyCode( event.key.code );
#else
								unsigned char iKey = TranslateKeyCode( keyCode );
#endif
								return m_Canvas->InputKey( iKey, bPressed );
							}
                        default:
                            break;
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
