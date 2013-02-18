#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class PageControl : public GUnit
{
	public:

		GWEN_CONTROL_INLINE( PageControl, GUnit )
		{
			Gwen::Controls::PageControl* control = new Gwen::Controls::PageControl( this );
			control->SetSize( 500, 300 );
			control->SetPageCount( 5 );
			control->onPageChanged.Add( this, &ThisClass::OnPageChanged );
			control->onFinish.Add( this, &ThisClass::OnFinish );
			// Page 0
			{
				Gwen::Controls::Button* pButton = new Gwen::Controls::Button( control->GetPage( 0 ) );
				pButton->Dock( Pos::Fill );
				pButton->SetText( "This button is fill docked on page 0" );
			}
			// Page 1
			{
				Gwen::Controls::Button* pButton = new Gwen::Controls::Button( control->GetPage( 1 ) );
				pButton->Dock( Pos::Top );
				pButton->SetText( "This button is top docked on page 1" );
			}
			// Page 2
			{
				Gwen::Controls::Button* pButton = new Gwen::Controls::Button( control->GetPage( 2 ) );
				pButton->SetSize( 400, 1000 );
				pButton->SetPos( 50, 50 );
				pButton->SetText( "This button is long to test scrolling (page 2)" );
			}
		}

		void OnPageChanged( Event::Info info )
		{
			UnitPrint( Utility::Format( L"PageControl Changed Page: %i", info.Integer ) );
		}

		void OnFinish( Event::Info info )
		{
			UnitPrint( L"Finish Pressed" );
		}
};



DEFINE_UNIT_TEST( PageControl, L"PageControl" );