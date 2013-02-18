
#pragma once

#include "Gwen/Gwen.h"
#include "ControlFactory.h"

namespace Gwen
{
	namespace ControlFactory
	{
		class Base;
		typedef std::list< ControlFactory::Base* > List;

		List & GetList();
		ControlFactory::Base* Find( const Gwen::String & name );
		Controls::Base* Clone( Controls::Base* pEnt, ControlFactory::Base* pFactory );

		class Property
		{
			public:

				typedef std::list<Property*> List;

				virtual Gwen::String			Name() = 0;
				virtual Gwen::String			Description() = 0;

				virtual Gwen::UnicodeString		GetValue( Gwen::Controls::Base* ctrl ) = 0;
				virtual void					SetValue( Gwen::Controls::Base* ctrl, const Gwen::UnicodeString & str ) = 0;

				virtual int						OptionNum() { return 0; }
				virtual Gwen::UnicodeString		OptionGet( int i ) { return L"";}

				virtual int						NumCount() { return 0; };
				virtual Gwen::String			NumName( int i ) { return "unknown"; };
				virtual float					NumGet( Gwen::Controls::Base* ctrl, int i ) { return 0.0f; };
				virtual void					NumSet( Gwen::Controls::Base* ctrl, int i, float f ) {};

				inline void NumSet( Gwen::Controls::Base* ctrl, const Gwen::String & str, float f )
				{
					for ( int i = 0; i < NumCount(); i++ )
					{
						if ( NumName( i ) == str ) { NumSet( ctrl, i, f ); }
					}
				};
		};

		class PropertyBool : public Property
		{
			public:

				static const Gwen::UnicodeString		True;
				static const Gwen::UnicodeString		False;

				virtual int						OptionNum() { return 2; }
				virtual Gwen::UnicodeString		OptionGet( int i ) { if ( i == 0 ) { return False; } return True; }
		};

		class Base
		{
			public:

				Base();

				virtual Gwen::String Name() = 0;
				virtual Gwen::String BaseName() = 0;

				virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent ) = 0;

				Base* GetBaseFactory();
				void AddProperty( Property* pProp );

				Property* GetProperty( const Gwen::String & name );
				void SetControlValue( Gwen::Controls::Base* ctrl, const Gwen::String & name, const Gwen::UnicodeString & str );

				const Property::List & Properties() { return m_Properties; }

				// Called when the control is drag and dropped onto the parent, even when just moving in the designer
				virtual void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child,
									   const Gwen::Point & pos );

				// Called when creating the control - param might be empty
				virtual void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child,
									   int iPage = 0 );

				// Called when a child is clicked on in an editor
				virtual bool ChildTouched( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* pChildControl ) { return false; };


				//
				// If a control is parented to a control with multiple sections, or pages
				// these functions are used to store which page they're on - on the child control
				//
				virtual void SetParentPage( Gwen::Controls::Base* ctrl, int i );
				virtual int GetParentPage( Gwen::Controls::Base* ctrl );

			protected:

				Property::List	m_Properties;
		};

	}

}

#define GWEN_CONTROL_FACTORY_CONSTRUCTOR( ThisName, BaseName )\
	typedef BaseName BaseClass;\
	typedef ThisName ThisClass;\
	ThisName() : BaseClass()

#define GWEN_CONTROL_FACTORY( ThisName ) \
    void GWENCONTROLFACTORY##ThisName()\
    {\
        new ThisName();\
    }

#define DECLARE_GWEN_CONTROL_FACTORY( ThisName )\
    extern void GWENCONTROLFACTORY##ThisName();\
    GWENCONTROLFACTORY##ThisName();

#define GWEN_CONTROL_FACTORY_PROPERTY( _name_, _description_ )\
	public:\
	Gwen::String Name() { return #_name_; }\
	Gwen::String Description() { return _description_; }


