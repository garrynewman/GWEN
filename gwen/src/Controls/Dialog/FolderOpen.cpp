/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/Gwen.h"
#include "Gwen/Platform.h"
#include "Gwen/Controls/Dialogs/FolderOpen.h"

using namespace Gwen;
using namespace Gwen::Dialogs;

void Gwen::Dialogs::FolderOpenEx( bool bUseSystem, const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler, Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	if ( bUseSystem && Gwen::Platform::FolderOpen( Name, StartPath, pHandler, fnCallback ) )
	{
		return;
	}

	//
	// TODO: SHOW GWEN FOLDER SELECTION DIALOG
	//
}