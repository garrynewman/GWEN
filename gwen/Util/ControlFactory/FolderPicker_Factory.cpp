
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		using namespace Gwen;

		class FolderPicker_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( FolderPicker_Factory, ControlFactory::Base )
				{
				}

				virtual Gwen::String Name()     { return "FolderPicker"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::FolderPicker* pControl = new Gwen::Controls::FolderPicker( parent );
					pControl->SetSize( 100, 20 );
					return pControl;
				}
		};

		GWEN_CONTROL_FACTORY( FolderPicker_Factory );

	}
}
