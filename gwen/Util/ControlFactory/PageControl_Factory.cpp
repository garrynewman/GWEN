
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

using namespace Gwen;

namespace Property 
{
	class NumPages: public ControlFactory::Property 
	{
		GWEN_CONTROL_FACTORY_PROPERTY( NumPages, "The number of pages we have" );

		UnicodeString GetValue( Controls::Base* ctrl )
		{
			return Utility::Format( L"%i", (int)gwen_cast<Controls::PageControl>(ctrl)->GetPageCount() );
		}

		void SetValue( Controls::Base* ctrl, const UnicodeString& str )
		{
			int num;
			if ( swscanf( str.c_str(), L"%i", &num ) != 1 ) return;

			gwen_cast<Controls::PageControl>(ctrl)->SetPageCount( num );
		}

	};

	class FinishName: public ControlFactory::Property 
	{
		GWEN_CONTROL_FACTORY_PROPERTY( FinishName, "The name of the finish button" );

		UnicodeString GetValue( Controls::Base* ctrl )
		{
			Gwen::Controls::PageControl* pControl = gwen_cast<Gwen::Controls::PageControl>( ctrl );
			return Utility::StringToUnicode( pControl->FinishButton()->GetName() );
		}

		void SetValue( Controls::Base* ctrl, const UnicodeString& str )
		{
			Gwen::Controls::PageControl* pControl = gwen_cast<Gwen::Controls::PageControl>( ctrl );
			pControl->FinishButton()->SetName( Utility::UnicodeToString( str ) );
		}

	};

}

class PageControl_Factory : public Gwen::ControlFactory::Base
{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( PageControl_Factory, Gwen::ControlFactory::Base )
		{
			AddProperty( new Property::NumPages() );
			AddProperty( new Property::FinishName() );
		}

		virtual Gwen::String Name(){ return "PageControl"; }
		virtual Gwen::String BaseName(){ return "Base"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::PageControl* pControl = new Gwen::Controls::PageControl( parent );
			pControl->SetSize( 300, 300 );
			pControl->SetPageCount( 1 );

			return pControl;
		}

		//
		// Called when a child is clicked on in the editor
		//
		virtual bool ChildTouched( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* pChildControl )
		{
			Gwen::Controls::PageControl* pControl = gwen_cast<Gwen::Controls::PageControl>( ctrl );

			if ( pChildControl == pControl->NextButton() ) { pControl->NextButton()->DoAction(); return true; }
			if ( pChildControl == pControl->BackButton() ){ pControl->BackButton()->DoAction(); return true; }

			return false;
		}

		//
		// A child is being dropped on this position.. set the parent properly
		//
		void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child, Gwen::Point& pos )
		{
			Gwen::Controls::PageControl* pControl = gwen_cast<Gwen::Controls::PageControl>(ctrl);
			AddChild( ctrl, child, pControl->GetPageNumber() );
		}

		void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child, int iPage )
		{
			Gwen::Controls::PageControl* pControl = gwen_cast<Gwen::Controls::PageControl>(ctrl);
			if ( !pControl->GetPage( iPage ) ) iPage = 0;

			SetParentPage( child, iPage );
			child->SetParent( pControl->GetPage( iPage ) );
		}
};


GWEN_CONTROL_FACTORY( PageControl_Factory );