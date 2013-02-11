
#include "Gwen/Util/ControlFactory.h"
#include "Gwen/Controls.h"

namespace Gwen {
namespace ControlFactory {

	using namespace Gwen;

	class VerticalSplitter_Factory : public Gwen::ControlFactory::Base
	{
	public:

		GWEN_CONTROL_FACTORY_CONSTRUCTOR( VerticalSplitter_Factory, Gwen::ControlFactory::Base )
		{

		}

		virtual Gwen::String Name(){ return "VerticalSplitter"; }
		virtual Gwen::String BaseName(){ return "Base"; }

		virtual Gwen::Controls::Base* CreateInstance( Gwen::Controls::Base* parent )
		{
			Gwen::Controls::SplitterVertical* pControl = new Gwen::Controls::SplitterVertical( parent );
			pControl->SetSize( 200, 200 );
			return pControl;
		}

		void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child, Gwen::Point& pos )
		{
			Gwen::Controls::SplitterVertical* pSplitter = gwen_cast<Gwen::Controls::SplitterVertical>(ctrl);
			child->SetParent( pSplitter );

			if ( pos.y < pSplitter->SplitterPos() )
				pSplitter->SetPanels( child, NULL );
			else
				pSplitter->SetPanels( NULL, child );
		}

		void AddChild( Gwen::Controls::Base* ctrl, Gwen::Controls::Base* child, int iPage )
		{
			child->SetParent( ctrl );
		}
	};


	GWEN_CONTROL_FACTORY( VerticalSplitter_Factory );

} }

