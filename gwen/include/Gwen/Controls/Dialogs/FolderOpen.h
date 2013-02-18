#pragma once
#ifndef GWEN_CONTROLS_DIALOGS_FOLDEROPEN_H
#define GWEN_CONTROLS_DIALOGS_FOLDEROPEN_H

#include "Gwen/Gwen.h"

namespace Gwen
{
	namespace Dialogs
	{
		// Usage:
		//
		// Gwen::Dialogs::FileOpen( true, "Open Map", "C:\my\folder\", "My Map Format|*.bmf", this, &MyClass::OpenFilename );
		//

		//
		// The REAL function.
		// If bUseSystem is used, it may use the system's modal dialog - which
		// will steal focus and pause the rest of GWEN until it's continued.
		//
		void GWEN_EXPORT FolderOpenEx( bool bUseSystem, const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler = NULL, Gwen::Event::Handler::FunctionWithInformation fnCallback = NULL );

		//
		// Templated function simply to avoid having to manually cast the callback function.
		//
		template< typename A>
		void FolderOpen( bool bUseSystem, const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler = NULL, A fnCallback = NULL )
		{
			FolderOpenEx( bUseSystem, Name, StartPath, pHandler, static_cast<Gwen::Event::Handler::FunctionWithInformation>( fnCallback ) );
		}


	}
}
#endif
