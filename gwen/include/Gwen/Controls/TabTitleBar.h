/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
	*/

#pragma once
#ifndef GWEN_CONTROLS_TABTITLEBAR_H
#define GWEN_CONTROLS_TABTITLEBAR_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/TabButton.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/DragAndDrop.h"
#include "Gwen/Skin.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT TabTitleBar : public Label
		{
		public:

			GWEN_CONTROL_INLINE(TabTitleBar, Label)
			{
				m_CurrentButton = 0;
				
				SetMouseInputEnabled(true);
				SetTextPadding(Gwen::Padding(5, 2, 5, 2));
				SetPadding(Gwen::Padding(1, 2, 1, 2));
				DragAndDrop_SetPackage(true, "TabWindowMove");
				
				// add other buttons here
				m_CloseButton = new Controls::Button(this);
				m_CloseButton->SetSize(20, 20);
				m_CloseButton->SetPos(0, 2);
				m_CloseButton->Dock(Pos::Right);
				m_CloseButton->SetText("x");
				m_CloseButton->onPress.Add(this, &ThisClass::OnCloseTab);
				m_CloseButton->SetHidden(true);
			}

			void Render(Skin::Base* skin)
			{
				skin->DrawTabTitleBar(this);
			}

			void DragAndDrop_StartDragging(Gwen::DragAndDrop::Package* pPackage, int x, int y)
			{
				DragAndDrop::SourceControl = GetParent();
				DragAndDrop::SourceControl->DragAndDrop_StartDragging(pPackage, x, y);
			}

			void UpdateFromTab(TabButton* pButton)
			{
				SetText(pButton->GetText());
				SizeToContents();
				m_CurrentButton = pButton;
				m_CloseButton->SetHidden(!pButton->IsClosable());
			}

		private:

			void OnCloseTab(Controls::Base* control)
			{
				if (m_CurrentButton)
				{
					auto page = m_CurrentButton->GetPage();
					m_CurrentButton->GetTabControl()->RemovePage(m_CurrentButton);
					page->DelayedDelete();
					m_CurrentButton->DelayedDelete();
					m_CurrentButton = 0;
				}
			}
			
			TabButton* m_CurrentButton;
			Button* m_CloseButton;
		};
	}
}
#endif
