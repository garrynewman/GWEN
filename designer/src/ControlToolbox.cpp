
#include "ControlToolbox.h"
#include "Gwen/Util/ControlFactory.h"

GWEN_CONTROL_CONSTRUCTOR( ControlToolbox )
{
	SetWidth( 150 );
	SetMargin( Margin( 5, 5, 5, 5 ) );
	SetText( "Controls" );

	Controls::Layout::Tile* pTileLayout = new Controls::Layout::Tile( this );
	pTileLayout->SetTileSize( 22, 22 );

	for ( ControlFactory::List::iterator it = ControlFactory::GetList().begin(); it != ControlFactory::GetList().end(); ++it )
	{
		ControlFactory::Base* pControlFactory = *it;

		if ( pControlFactory->Name() == "DesignerCanvas" ) continue;

		Controls::Button* pButton = new Controls::Button( pTileLayout );
		pButton->SetSize( 20, 20 );
		pButton->SetToolTip( pControlFactory->Name() );
		pButton->SetImage( "img/controls/" + pControlFactory->Name() + ".png" );
		pButton->SetShouldDrawBackground( false );

		//
		// Make drag and droppable. Pass the ControlFactory as the userdata
		//
		pButton->DragAndDrop_SetPackage( true, "ControlSpawn", pControlFactory );
	}

}
