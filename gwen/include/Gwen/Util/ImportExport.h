
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"
#include "Gwen/Util/ControlFactory.h"

using namespace Gwen;

namespace Gwen
{
	namespace ImportExport
	{
		class Base
		{
			public:

				Base();

				virtual Gwen::String Name() = 0;

				virtual bool CanImport() = 0;
				virtual void Import( Gwen::Controls::Base* pRoot, const Gwen::String & strFilename ) = 0;

				virtual bool CanExport() = 0;
				virtual void Export( Gwen::Controls::Base* pRoot, const Gwen::String & strFilename ) = 0;

			public:

				typedef std::list<ImportExport::Base*> List;

			protected:

		};

		ImportExport::Base::List & GetList();
		ImportExport::Base* Find( Gwen::String strName );

		namespace Tools
		{
			ControlList GetExportableChildren( Gwen::Controls::Base* pRoot );
		}
	}
}

#define GWEN_IMPORTEXPORT( name )\
void GWENIMPORTEXPORT_##name()\
{\
	static name my_instance;\
}

#define DECLARE_GWEN_IMPORTEXPORT( name )\
void GWENIMPORTEXPORT_##name();\
GWENIMPORTEXPORT_##name();