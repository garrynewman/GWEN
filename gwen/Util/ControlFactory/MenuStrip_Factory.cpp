
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

class MenuStrip_Factory : public Gwen::ControlFactory::Base
{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( MenuStrip_Factory, Gwen::ControlFactory::Base )
		{

		}

		virtual Gwen::String Name(){ return "MenuStrip"; }
		virtual Gwen::String BaseName(){ return "Base"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::MenuStrip* pControl = new Gwen::Controls::MenuStrip( parent );
			return pControl;
		}
};


GWEN_CONTROL_FACTORY( MenuStrip_Factory );