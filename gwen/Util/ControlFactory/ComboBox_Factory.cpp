
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen {
namespace ControlFactory {

	class ComboBox_Factory : public Gwen::ControlFactory::Base
	{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( ComboBox_Factory, Gwen::ControlFactory::Base )
		{

		}

		virtual Gwen::String Name()     { return "ComboBox"; }
		virtual Gwen::String BaseName() { return "Base"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::ComboBox* pControl = new Gwen::Controls::ComboBox( parent );
			pControl->SetSize( 100, 20 );
			return pControl;
		}
	};


	GWEN_CONTROL_FACTORY( ComboBox_Factory );

} }
