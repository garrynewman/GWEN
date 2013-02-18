#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Controls/RadioButtonController.h"

using namespace Gwen;

class RadioButton : public GUnit
{
	public:

		GWEN_CONTROL_INLINE( RadioButton, GUnit )
		{
			Gwen::Controls::RadioButtonController* rc = new Gwen::Controls::RadioButtonController( this );
			rc->AddOption( "Option 1" );
			rc->AddOption( "Option 2" );
			rc->AddOption( "Option 3" );
			rc->AddOption( L"\u0627\u0644\u0622\u0646 \u0644\u062D\u0636\u0648\u0631" );
			rc->SetBounds( 30, 30, 200, 200 );
			rc->onSelectionChange.Add( this, &RadioButton::OnChange );
		}

		void OnChange( Controls::Base* pControl )
		{
			Gwen::Controls::RadioButtonController* rc = ( Gwen::Controls::RadioButtonController* ) pControl;
			Gwen::Controls::LabeledRadioButton* pSelected = rc->GetSelected();
			UnitPrint( Utility::Format( L"RadioButton changed (using 'OnChange' event)\n Chosen Item: '%ls'", pSelected->GetLabel()->GetText().GetUnicode().c_str() ) );
		}

};



DEFINE_UNIT_TEST( RadioButton, L"RadioButton" );
