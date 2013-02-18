
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		namespace Properties
		{

			class ImageName : public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( ImageName, "The path to the image (relative to .exe)" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return gwen_cast<Controls::ImagePanel> ( ctrl )->GetImage();
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						gwen_cast<Controls::ImagePanel> ( ctrl )->SetImage( str );
					}

			};

			class Stretch: public ControlFactory::PropertyBool
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Stretch, "The path to the image (relative to .exe)" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						if ( gwen_cast<Controls::ImagePanel> ( ctrl )->GetStretch() ) { return True; }

						return False;
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						bool bTrue = ( str == True );
						gwen_cast<Controls::ImagePanel> ( ctrl )->SetStretch( bTrue );
					}

			};

		}

		class Image_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( Image_Factory, ControlFactory::Base )
				{
					AddProperty( new Properties::ImageName() );
					AddProperty( new Properties::Stretch() );
				}

				virtual Gwen::String Name()     { return "ImagePanel"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::ImagePanel* pControl = new Gwen::Controls::ImagePanel( parent );
					pControl->SetSize( 100, 20 );
					return pControl;
				}
		};

		GWEN_CONTROL_FACTORY( Image_Factory );

	}
}
