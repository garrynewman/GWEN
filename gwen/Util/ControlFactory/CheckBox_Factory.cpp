
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		class CheckBox_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( CheckBox_Factory, Gwen::ControlFactory::Base )
				{
				}

				virtual Gwen::String Name() { return "CheckBox"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::CheckBox* pControl = new Gwen::Controls::CheckBox( parent );
					return pControl;
				}
		};

		GWEN_CONTROL_FACTORY( CheckBox_Factory );



		namespace Properties
		{
			class CheckboxText: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( CheckboxText, "The text, or label of the control" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return gwen_cast<Controls::CheckBoxWithLabel>( ctrl )->Label()->GetText().GetUnicode();
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						gwen_cast<Controls::CheckBoxWithLabel>( ctrl )->Label()->SetText( str );
					}

			};
		}

		class CheckBoxWithLabel_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( CheckBoxWithLabel_Factory, Gwen::ControlFactory::Base )
				{
					AddProperty( new Properties::CheckboxText() );
				}

				virtual Gwen::String Name()     { return "CheckBoxWithLabel"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::CheckBoxWithLabel* pControl = new Gwen::Controls::CheckBoxWithLabel( parent );
					return pControl;
				}
		};


		GWEN_CONTROL_FACTORY( CheckBoxWithLabel_Factory );

	}
}
