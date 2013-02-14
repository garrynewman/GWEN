#include <SFML/Graphics.hpp>
#include <cmath>

#include "Gwen/Renderers/SFML2.h"
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
	sf::RenderWindow App( sf::VideoMode( 1004, 650, 32 ), "GWEN: SFML2");

	Gwen::Renderer::SFML2 GwenRenderer( App );

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

	pCanvas->SetSize( App.getSize().x, App.getSize().y );
	pCanvas->SetDrawBackground( true );
	pCanvas->SetBackgroundColor( Gwen::Color( 150, 170, 170, 255 ) );

	//
	// Create our unittest control (which is a Window with controls in it)
	//
	UnitTest* pUnit = new UnitTest( pCanvas );

	//
	// Create an input processor
	//
	Gwen::Input::SFML GwenInput;
	GwenInput.Initialize( pCanvas );

	while ( App.isOpen() )
	{
		// Handle events
		sf::Event Event;

		while ( App.pollEvent(Event) )
		{
			// Window closed or escape key pressed : exit
			if ((Event.type == sf::Event::Closed) || 
				((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape)))
			{
				App.close();
				break;
			}
			else if (Event.type == sf::Event::Resized)
			{
				pCanvas->SetSize(Event.size.width, Event.size.height);
			}

			GwenInput.ProcessMessage( Event );
		}

		// Clear the window

        App.clear();		
		pCanvas->RenderCanvas();		
    	App.display();
	}

	return EXIT_SUCCESS;
}
