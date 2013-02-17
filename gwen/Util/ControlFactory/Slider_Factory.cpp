
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"


namespace Gwen
{
	namespace ControlFactory
	{

		using namespace Gwen;

		namespace Properties
		{

			class Min: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Min, "The minimum value" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Gwen::Utility::Format( L"%f", ( int ) gwen_cast<Controls::Slider>( ctrl )->GetMin() );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						float val;

						if ( swscanf( str.c_str(), L"%f", &val ) != 1 ) { return; }

						if ( val == gwen_cast<Controls::Slider>( ctrl )->GetMin() ) { return; }

						gwen_cast<Controls::Slider>( ctrl )->SetRange( val, gwen_cast<Controls::Slider>( ctrl )->GetMax() );
					}

			};

			class Max: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Max, "The max value" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Gwen::Utility::Format( L"%f", ( int ) gwen_cast<Controls::Slider>( ctrl )->GetMax() );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						float val;

						if ( swscanf( str.c_str(), L"%f", &val ) != 1 ) { return; }

						if ( val == gwen_cast<Controls::Slider>( ctrl )->GetMax() ) { return; }

						gwen_cast<Controls::Slider>( ctrl )->SetRange( gwen_cast<Controls::Slider>( ctrl )->GetMin(), val );
					}

			};
		}

		class HorizontalSlider_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( HorizontalSlider_Factory, Gwen::ControlFactory::Base )
				{
					AddProperty( new Properties::Min() );
					AddProperty( new Properties::Max() );
				}

				virtual Gwen::String Name() { return "HorizontalSlider"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::HorizontalSlider* pControl = new Gwen::Controls::HorizontalSlider( parent );
					pControl->SetSize( 100, 20 );
					pControl->SetRange( 0, 1 );
					return pControl;
				}
		};


		GWEN_CONTROL_FACTORY( HorizontalSlider_Factory );

	}
}
