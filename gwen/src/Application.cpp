/*
	GWEN
	Copyright (c) 2022 Matthew B.
	See license in Gwen.h
*/

#include "Gwen/Application.h"

#ifndef _win32
#include <signal.h>
#endif

using namespace Gwen;

namespace Gwen
{
	Gwen::Application* gApplication = 0;
	
	#ifndef _win32
	void signal_handler(int sig)
	{
		gApplication->RequestQuit();
	}
	#endif
	
	Application::Application()
	{
		if (gApplication)
		{
			printf("Cannot instantiate multiple Gwen::Applications per process...\n");
			throw 7;
		}
		gApplication = this;
		
#ifndef _win32
		signal(SIGINT, signal_handler); 
#endif
	}
}
