
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		using namespace Gwen;

		namespace Properties
		{
			class ControlName : public Gwen::ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( ControlName, "The control's name" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Utility::Format( L"%s", ctrl->GetName().c_str() );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						ctrl->SetName( Gwen::Utility::UnicodeToString( str ) );
					}
			};

			class Position : public Gwen::ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Position, "Sets the position of the control" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Utility::Format( L"%i %i", ctrl->X(), ctrl->Y() );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						int x, y;

						if ( swscanf( str.c_str(), L"%i %i", &x, &y ) != 2 ) { return; }

						ctrl->SetPos( x, y );
					}

					int NumCount() { return 2; };

					Gwen::String NumName( int i )
					{
						if ( i == 0 ) { return "x"; }

						return "y";
					}

					float NumGet( Controls::Base* ctrl, int i )
					{
						if ( i == 0 ) { return ctrl->X(); }

						return ctrl->Y();
					}

					void NumSet( Controls::Base* ctrl, int i, float f )
					{
						if ( i == 0 )   { ctrl->SetPos( f, ctrl->Y() ); }
						else            { ctrl->SetPos( ctrl->X(), f ); }
					}
			};

			class Margin : public Gwen::ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Margin, "Sets the margin of a docked control" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						Gwen::Margin m = ctrl->GetMargin();
						return Utility::Format( L"%i %i %i %i", m.left, m.top, m.right, m.bottom );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						Gwen::Margin m;

						if ( swscanf( str.c_str(), L"%i %i %i %i", &m.left, &m.top, &m.right, &m.bottom ) != 4 ) { return; }

						ctrl->SetMargin( m );
					}

					int NumCount() { return 4; };

					Gwen::String NumName( int i )
					{
						if ( i == 0 ) { return "left"; }

						if ( i == 1 ) { return "top"; }

						if ( i == 2 ) { return "right"; }

						return "bottom";
					}

					float NumGet( Controls::Base* ctrl, int i )
					{
						Gwen::Margin m = ctrl->GetMargin();

						if ( i == 0 ) { return m.left; }

						if ( i == 1 ) { return m.top; }

						if ( i == 2 ) { return m.right; }

						return m.bottom;
					}

					void NumSet( Controls::Base* ctrl, int i, float f )
					{
						Gwen::Margin m = ctrl->GetMargin();

						if ( i == 0 ) { m.left = f; }

						if ( i == 1 ) { m.top = f; }

						if ( i == 2 ) { m.right = f; }

						if ( i == 3 ) { m.bottom = f; }

						ctrl->SetMargin( m );
					}
			};

			class Size: public Gwen::ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Size, "The with and height of the control" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Utility::Format( L"%i %i", ctrl->Width(), ctrl->Height() );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						int w, h;

						if ( swscanf( str.c_str(), L"%i %i", &w, &h ) != 2 ) { return; }

						ctrl->SetSize( w, h );
					}

					int NumCount() { return 2; };

					Gwen::String NumName( int i )
					{
						if ( i == 0 ) { return "w"; }

						return "h";
					}

					float NumGet( Controls::Base* ctrl, int i )
					{
						if ( i == 0 ) { return ctrl->Width(); }

						return ctrl->Height();
					}

					void NumSet( Controls::Base* ctrl, int i, float f )
					{
						if ( i == 0 )   { ctrl->SetSize( f, ctrl->Height() ); }
						else            { ctrl->SetSize( ctrl->Width(), f ); }
					}
			};

			class Dock : public Gwen::ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Dock, "How the control is to be docked" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						switch ( ctrl->GetDock() )
						{
							case Pos::Left:
								return L"Left";

							case Pos::Fill:
								return L"Fill";

							case Pos::Right:
								return L"Right";

							case Pos::Top:
								return L"Top";

							case Pos::Bottom:
								return L"Bottom";
						}

						return L"None";
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						if ( str == L"Fill" ) { ctrl->Dock( Pos::Fill ); }

						if ( str == L"Left" ) { ctrl->Dock( Pos::Left ); }

						if ( str == L"Right" ) { ctrl->Dock( Pos::Right ); }

						if ( str == L"Top" ) { ctrl->Dock( Pos::Top ); }

						if ( str == L"Bottom" ) { ctrl->Dock( Pos::Bottom ); }

						if ( str == L"None" ) { ctrl->Dock( Pos::None ); }
					}

					int OptionNum() { return 6; }

					Gwen::UnicodeString OptionGet( int i )
					{
						if ( i == 0 ) { return L"None"; }

						if ( i == 1 ) { return L"Left"; }

						if ( i == 2 ) { return L"Right"; }

						if ( i == 3 ) { return L"Top"; }

						if ( i == 4 ) { return L"Bottom"; }

						return L"Fill";
					}
			};

		} // namespace Properties


		class Base_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( Base_Factory, Gwen::ControlFactory::Base )
				{
					AddProperty( new Properties::ControlName() );
					AddProperty( new Properties::Dock() );
					AddProperty( new Properties::Position() );
					AddProperty( new Properties::Size() );
					AddProperty( new Properties::Margin() );
				}

				virtual Gwen::String Name() { return "Base"; }
				virtual Gwen::String BaseName() { return ""; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::Base* pControl = new Gwen::Controls::Base( parent );
					pControl->SetSize( 100, 100 );
					return pControl;
				}
		};


		GWEN_CONTROL_FACTORY( Base_Factory );

	}
}
