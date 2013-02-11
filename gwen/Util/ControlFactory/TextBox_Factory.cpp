
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen {
namespace ControlFactory {

	class TextBox_Factory : public Gwen::ControlFactory::Base
	{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( TextBox_Factory, Gwen::ControlFactory::Base )
		{
		}

		virtual Gwen::String Name(){ return "TextBox"; }
		virtual Gwen::String BaseName(){ return "Label"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::TextBox* pControl = new Gwen::Controls::TextBox( parent );
			pControl->SetSize( 100, 20 );
			pControl->SetText( "" );
			return pControl;
		}
	};


	class TextBoxMultiLine_Factory : public Gwen::ControlFactory::Base
	{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( TextBoxMultiLine_Factory, Gwen::ControlFactory::Base )
		{
		}

		virtual Gwen::String Name(){ return "TextBoxMultiline"; }
		virtual Gwen::String BaseName(){ return "TextBox"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::TextBoxMultiline* pControl = new Gwen::Controls::TextBoxMultiline( parent );
			pControl->SetSize( 100, 50 );
			pControl->SetText( "" );
			return pControl;
		}
	};


	GWEN_CONTROL_FACTORY( TextBox_Factory );
	GWEN_CONTROL_FACTORY( TextBoxMultiLine_Factory );

} }

