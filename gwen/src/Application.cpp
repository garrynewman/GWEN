/*
	GWEN
	Copyright (c) 2022 Matthew B.
	See license in Gwen.h
*/

#include "Gwen/Application.h"
#include "Gwen/Renderers/OpenGL.h"

#include <Gwen/Platform.h>
#include <Gwen/Skins/TexturedBase.h>

#ifndef _win32
#include <signal.h>
#endif

using namespace Gwen;

namespace Gwen
{
	Gwen::BaseApplication* gApplication = 0;
	
#ifndef _win32
	void signal_handler(int sig)
	{
		gApplication->RequestQuit();
	}
#endif
}
