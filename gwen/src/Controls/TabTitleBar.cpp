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
		auto data = m_CurrentButton->UserData.Get<TabReturnButtonData*>("return_data");
		m_CurrentButton->UserData.Set<TabReturnButtonData*>("return_data", 0);
		
		int num_tabs = m_CurrentButton->GetTabControl()->TabCount();
		data->dock->GetRight()->GetTabControl()->AddPage(m_CurrentButton);
				
		WindowCanvas* canv = dynamic_cast<WindowCanvas*>(data->window);
		if (num_tabs == 1)
		{
			if (canv)
			{
				canv->InputQuit();
			}
			else
			{
				static_cast<WindowControl*>(data->window)->CloseButtonPressed();
			}
		}
			
		delete data;
	
		return;
	}
	
	// Create a new tab control in that window so we can properly move ourself into it
	auto dcontrol = m_CurrentButton->PopOut();
	
	Invalidate();
				
	m_CurrentButton = 0;
}



void TabTitleBar::OnCloseTab(Controls::Base* control)
{	
	if (m_PopOutIsReturn)
	{	
		auto data = m_CurrentButton->UserData.Get<TabReturnButtonData*>("return_data");
		m_CurrentButton->UserData.Set<TabReturnButtonData*>("return_data", 0);
		
		int num_tabs = m_CurrentButton->GetTabControl()->TabCount();
				
		WindowCanvas* canv = dynamic_cast<WindowCanvas*>(data->window);
		if (num_tabs == 1)
		{
			if (canv)
			{
				canv->InputQuit();
			}
			else
			{
				static_cast<WindowControl*>(data->window)->CloseButtonPressed();
			}
		}
			
		delete data;
		return;
	}
	
	if (m_CurrentButton)
	{
		auto page = m_CurrentButton->GetPage();
		m_CurrentButton->GetTabControl()->RemovePage(m_CurrentButton);
		page->DelayedDelete();
		m_CurrentButton->DelayedDelete();
		m_CurrentButton = 0;
	}
}
