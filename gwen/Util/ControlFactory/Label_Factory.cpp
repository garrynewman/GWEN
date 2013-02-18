
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen
{
	namespace ControlFactory
	{

		using namespace Gwen;

		namespace Properties
		{
			class Text: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Text, "The text, or label of the control" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						UnicodeString str = gwen_cast<Controls::Label> ( ctrl )->GetText().GetUnicode();
						Gwen::Utility::Replace<UnicodeString> ( str, L"\n", L"\\n" );
						Gwen::Utility::Replace<UnicodeString> ( str, L"\t", L"\\t" );
						return str;
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						UnicodeString strOut = str;
						Gwen::Utility::Replace<UnicodeString> ( strOut, L"\\n", L"\n" );
						Gwen::Utility::Replace<UnicodeString> ( strOut, L"\\t", L"\t" );
						gwen_cast<Controls::Label> ( ctrl )->SetText( strOut );
					}

			};

			class Font: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Font, "The font name" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return gwen_cast<Controls::Label> ( ctrl )->GetFont()->facename;
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						if ( str == L"" ) { return; }

						Gwen::Font* pFont = gwen_cast<Controls::Label> ( ctrl )->GetFont();
						gwen_cast<Controls::Label> ( ctrl )->SetFont( str, pFont->size, pFont->bold );
					}

			};

			class FontSize: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( FontSize, "The font size" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						return Gwen::Utility::Format( L"%i", ( int ) gwen_cast<Controls::Label> ( ctrl )->GetFont()->size );
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						int size;

						if ( swscanf( str.c_str(), L"%i", &size ) != 1 ) { return; }

						Gwen::Font* pFont = gwen_cast<Controls::Label> ( ctrl )->GetFont();

						if ( size == pFont->size ) { return; }

						gwen_cast<Controls::Label> ( ctrl )->SetFont( pFont->facename, size, pFont->bold );
					}

			};

			class FontBold: public ControlFactory::PropertyBool
			{
					GWEN_CONTROL_FACTORY_PROPERTY( FontBold, "The font bold" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						if ( gwen_cast<Controls::Label> ( ctrl )->GetFont()->bold ) { return True; }

						return False;
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						bool bTrue = ( str == True );
						Gwen::Font* pFont = gwen_cast<Controls::Label> ( ctrl )->GetFont();

						if ( bTrue == pFont->bold ) { return; }

						gwen_cast<Controls::Label> ( ctrl )->SetFont( pFont->facename, pFont->size, bTrue ? true : false );
					}

			};

			class Wrap: public ControlFactory::PropertyBool
			{
					GWEN_CONTROL_FACTORY_PROPERTY( Wrap, "Wrap the text" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						if ( gwen_cast<Controls::Label> ( ctrl )->Wrap() ) { return True; }

						return False;
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						bool bTrue = ( str == True );

						if ( bTrue == gwen_cast<Controls::Label> ( ctrl )->Wrap() ) { return; }

						gwen_cast<Controls::Label> ( ctrl )->SetWrap( bTrue );
					}

			};

			class VerticalAlign: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( VerticalAlign, "VerticalAlign" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::Top )	{ return L"Top"; }

						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::CenterV )	{ return L"Center"; }

						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::Bottom )	{ return L"Bottom"; }

						return L"Top";
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						int iAlign = gwen_cast<Controls::Label> ( ctrl )->GetAlignment();
						iAlign &= ~Pos::Top;
						iAlign &= ~Pos::CenterV;
						iAlign &= ~Pos::Bottom;

						if ( str == L"Top" )	{ gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::Top ); }

						if ( str == L"Center" ) { gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::CenterV ); }

						if ( str == L"Bottom" ) { gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::Bottom ); }
					}

					int	OptionNum() { return 3; }

					Gwen::UnicodeString	OptionGet( int i )
					{
						if ( i == 0 ) { return L"Top"; }

						if ( i == 1 ) { return L"Center"; }

						return L"Bottom";
					}

			};

			class HorizontalAlign: public ControlFactory::Property
			{
					GWEN_CONTROL_FACTORY_PROPERTY( HorizontalAlign, "HorizontalAlign" );

					UnicodeString GetValue( Controls::Base* ctrl )
					{
						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::Left )	{ return L"Left"; }

						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::CenterH )	{ return L"Center"; }

						if ( gwen_cast<Controls::Label> ( ctrl )->GetAlignment() & Pos::Right )	{ return L"Right"; }

						return L"Left";
					}

					void SetValue( Controls::Base* ctrl, const UnicodeString & str )
					{
						int iAlign = gwen_cast<Controls::Label> ( ctrl )->GetAlignment();
						iAlign &= ~Pos::Left;
						iAlign &= ~Pos::CenterH;
						iAlign &= ~Pos::Right;

						if ( str == L"Left" )	{ gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::Left ); }

						if ( str == L"Center" ) { gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::CenterH ); }

						if ( str == L"Right" ) { gwen_cast<Controls::Label> ( ctrl )->SetAlignment( iAlign | Pos::Right ); }
					}

					int	OptionNum() { return 3; }

					Gwen::UnicodeString	OptionGet( int i )
					{
						if ( i == 0 ) { return L"Left"; }

						if ( i == 1 ) { return L"Center"; }

						return L"Right";
					}

			};

		} // namespace Properties

		class Label_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( Label_Factory, ControlFactory::Base )
				{
					AddProperty( new Properties::Text() );
					AddProperty( new Properties::Wrap() );
					AddProperty( new Properties::Font() );
					AddProperty( new Properties::FontSize() );
					AddProperty( new Properties::FontBold() );
					AddProperty( new Properties::VerticalAlign() );
					AddProperty( new Properties::HorizontalAlign() );
				}

				virtual Gwen::String Name()     { return "Label"; }
				virtual Gwen::String BaseName() { return "Base"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::Label* pControl = new Gwen::Controls::Label( parent );
					pControl->SetSize( 100, 20 );
					pControl->SetText( "New Label" );
					return pControl;
				}
		};

		GWEN_CONTROL_FACTORY( Label_Factory );

		class LabelClickable_Factory : public Gwen::ControlFactory::Base
		{
			public:

				GWEN_CONTROL_FACTORY_CONSTRUCTOR( LabelClickable_Factory, ControlFactory::Base )
				{
				}

				virtual Gwen::String Name()     { return "LabelClickable"; }
				virtual Gwen::String BaseName() { return "Label"; }

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
				{
					Gwen::Controls::Label* pControl = new Gwen::Controls::LabelClickable( parent );
					pControl->SetSize( 100, 20 );
					pControl->SetText( "LabelClickable" );
					return pControl;
				}
		};

		GWEN_CONTROL_FACTORY( LabelClickable_Factory );

	}
}
