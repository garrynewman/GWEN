
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class DocumentCanvas;

class Properties : public Controls::Base 
{
	GWEN_CONTROL( Properties, Controls::Base );

		void WatchCanvas( DocumentCanvas* pCanvas );

	protected:

		void OnCanvasSelectionChanged( Event::Info info );
		void OnPropertiesChanged( Event::Info info );

		void OnPropertyChanged( Event::Info info );

		void AddPropertiesFromControl( Controls::Base* pControl, bool bAllowDifferent );

		DocumentCanvas*				m_pCanvas;
		Controls::PropertyTree*		m_Props;
		ControlList					m_SelectedControls;


};