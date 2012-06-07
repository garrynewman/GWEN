/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Controls/Rectangle.h"

using namespace Gwen;
using namespace Gwen::Controls;


GWEN_CONTROL_CONSTRUCTOR( Rectangle )
{
	m_Color = Gwen::Color( 255, 255, 255, 255 );
}

void Rectangle::Render( Skin::Base* skin )
{
	skin->GetRender()->SetDrawColor( m_Color );
	skin->GetRender()->DrawFilledRect( GetRenderBounds() ); 
}
