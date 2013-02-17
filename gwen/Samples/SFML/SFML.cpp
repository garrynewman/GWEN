#include <SFML/Graphics.hpp>
#include <cmath>

#include "Gwen/Renderers/SFML.h"
#include "Gwen/Input/SFML.h"

#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
	// Create the window of the application
	sf::RenderWindow App( sf::VideoMode( 1004, 650, 32 ), "GWEN: SFML", sf::Style::Close );
	Gwen::Renderer::SFML GwenRenderer( App );
	//
	// Create a GWEN skin
	//
	//Gwen::Skin::Simple skin;
	//skin.SetRender( &GwenRenderer );
	Gwen::Skin::TexturedBase skin( &GwenRenderer );
	skin.Init( "DefaultSkin.png" );
	// The fonts work differently in SFML - it can't use
	// system fonts. So force the skin to use a local one.
	skin.SetDefaultFont( L"OpenSans.ttf", 11 );
	//
	// Create a Canvas (it's root, on which all other GWEN panels are created)
	//
	Gwen::Controls::Canvas* pCanvas = new Gwen::Controls::Canvas( &skin );
#if SFML_VERSION_MAJOR == 2
	pCanvas->SetSize( App.getSize().x, App.getSize().y );
#else
	pCanvas->SetSize( App.GetWidth(), App.GetHeight() );
#endif
	pCanvas->SetDrawBackground( true );
	pCanvas->SetBackgroundColor( Gwen::Color( 150, 170, 170, 255 ) );
	//
	// Create our unittest control (which is a Window with controls in it)
	//
	UnitTest* pUnit = new UnitTest( pCanvas );
	//pUnit->SetPos( 10, 10 );
	//
	// Create an input processor
	//
	Gwen::Input::SFML GwenInput;
	GwenInput.Initialize( pCanvas );
#if SFML_VERSION_MAJOR == 2

	while ( App.isOpen() )
#else
	while ( App.IsOpened() )
#endif
	{
		// Handle events
		sf::Event Event;
#if SFML_VERSION_MAJOR == 2

		while ( App.pollEvent( Event ) )
#else
		while ( App.GetEvent( Event ) )
#endif
		{
			// Window closed or escape key pressed : exit
#if SFML_VERSION_MAJOR == 2
			if ( ( Event.type == sf::Event::Closed ) ||
					( ( Event.type == sf::Event::KeyPressed ) && ( Event.key.code == sf::Keyboard::Escape ) ) )
#else
			if ( ( Event.Type == sf::Event::Closed ) ||
					( ( Event.Type == sf::Event::KeyPressed ) && ( Event.Key.Code == sf::Key::Escape ) ) )
#endif
			{
#if SFML_VERSION_MAJOR == 2
				App.close();
#else
				App.Close();
#endif
				break;
			}

			GwenInput.ProcessMessage( Event );
		}

		// Clear the window
#if SFML_VERSION_MAJOR == 2
		App.clear();
#else
		App.Clear();
#endif
		pCanvas->RenderCanvas();
#if SFML_VERSION_MAJOR == 2
		App.display();
#else
		App.Display();
#endif
	}

	return EXIT_SUCCESS;
}
