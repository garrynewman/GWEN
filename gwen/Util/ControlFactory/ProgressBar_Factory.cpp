
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen {
namespace ControlFactory {

    using namespace Gwen;
    
    namespace Properties
    {
        class CycleSpeed: public ControlFactory::Property
        {
            GWEN_CONTROL_FACTORY_PROPERTY( CycleSpeed, "" );
            
            UnicodeString GetValue( Controls::Base* ctrl )
            {
                return Utility::Format( L"%f", (float)gwen_cast<Controls::ProgressBar>(ctrl)->GetCycleSpeed() );
            }
            
            void SetValue( Controls::Base* ctrl, const UnicodeString& str )
            {
                float num;
                if ( swscanf( str.c_str(), L"%f", &num ) != 1 ) return;
                
                gwen_cast<Controls::ProgressBar>(ctrl)->SetCycleSpeed( num );
            }
            
        };
        
    }
    
    class ProgressBar_Factory : public Gwen::ControlFactory::Base
    {
	public:
        
		GWEN_CONTROL_FACTORY_CONSTRUCTOR( ProgressBar_Factory, Gwen::ControlFactory::Base )
		{
			AddProperty( new Properties::CycleSpeed() );
		}
        
		virtual Gwen::String Name(){ return "ProgressBar"; }
		virtual Gwen::String BaseName(){ return "Base"; }
        
		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::ProgressBar* pControl = new Gwen::Controls::ProgressBar( parent );
			pControl->SetSize( 200, 20 );
			return pControl;
		}
    };
    
    
    GWEN_CONTROL_FACTORY( ProgressBar_Factory );

} }
