
#include <stdlib.h>
#include <memory.h>

#include <Gwen/Gwen.h>
#include <Gwen/Application.h>
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Renderers/OpenGL.h"


//
// Program starts here
//
int main(int argc, char** args)
{
	//skin->Init("DefaultSkin.png");
	//skin->SetDefaultFont(L"Open Sans", 14);
	
	// everything is automatically released when this goes out of scope
	Gwen::Application<Gwen::Renderer::OpenGL> app;
	app.SetDefaultFont(L"Microsoft Sans Serif", 10);

	//
	// Create our unittest control
	//
	UnitTest* pUnit = new UnitTest( app.AddWindow("Gwen's Cross Platform Application Example", 700, 500) );
	pUnit->SetPos( 0, 0 );
	
	// Wait for exit
	app.Spin();

	return 0;
}
