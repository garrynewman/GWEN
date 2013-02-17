
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		class ListBox_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( ListBox_Factory, Gwen::ControlFactory::Base )
				{
				}

				virtual Gwen::String Name()     { return "ListBox"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::ListBox* pControl = new Gwen::Controls::ListBox( parent );
					pControl->SetSize( 100, 100 );
					return pControl;
				}
		};


		GWEN_CONTROL_FACTORY( ListBox_Factory );

	}
}
