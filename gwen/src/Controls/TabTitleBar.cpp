/*
	GWEN
	Copyright (c) 2022 Matthew B and Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/TabTitleBar.h"
#include "Gwen/Controls/DockedTabControl.h"
#include "Gwen/Controls/WindowCanvas.h"
#include "Gwen/Controls/WindowControl.h"

using namespace Gwen;
using namespace Gwen::Controls;


void TabTitleBar::OnPopoutOrReturn(Controls::Base* control)
{
	if (!m_CurrentButton)
	{
		return;
	}
				
	if (m_PopOutIsReturn)
	{	
		m_CurrentButton->Return();
		m_CurrentButton = 0;
		return;
	}
	
	// Create a new tab control in that window so we can properly move ourself into it
	auto dcontrol = m_CurrentButton->PopOut();
	
	Invalidate();
	
	m_CurrentButton = 0;
}

void TabTitleBar::OnCloseTab(Controls::Base* control)
{	
	if (m_CurrentButton)
	{
		m_CurrentButton->Close();
		m_CurrentButton = 0;
	}
}
