
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen {
namespace ControlFactory {


	class DesignerCanvas_Factory : public Gwen::ControlFactory::Base
	{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( DesignerCanvas_Factory, Gwen::ControlFactory::Base )
		{

		}

		virtual Gwen::String Name()     { return "DesignerCanvas"; }
		virtual Gwen::String BaseName() { return "Base"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			return NULL;
		}
	};


	GWEN_CONTROL_FACTORY( DesignerCanvas_Factory );

} }
