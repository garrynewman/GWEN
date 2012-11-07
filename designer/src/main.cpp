#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/Renderers/DirectX9.h"
#include "DesignerBase.h"

int main()
{
	Gwen::Renderer::DirectX9	renderer;
	Gwen::Skin::TexturedBase	skin( &renderer );

	Gwen::Controls::WindowCanvas window_canvas( -1, -1, 960, 600, &skin, "GWEN Designer" );
	skin.Init( "img/DesignerSkin.png" );

	DesignerBase* pDesignerBase = new DesignerBase( &window_canvas );

	while ( !window_canvas.WantsQuit() )
	{
		window_canvas.DoThink();
	}
}