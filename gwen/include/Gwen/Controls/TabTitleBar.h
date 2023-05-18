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
#include "Gwen/Controls/DockBase.h"
#include "Gwen/Application.h"
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
				m_PopOutIsReturn = false;
				
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
				m_CloseButton->SetHidden(false);
				
				m_PopOutButton = new Controls::Button(this);
				m_PopOutButton->SetSize(20, 20);
				m_PopOutButton->SetPos(0, 2);
				m_PopOutButton->Dock(Pos::Right);
				m_PopOutButton->SetText("^");
				m_PopOutButton->onPress.Add(this, &ThisClass::OnPopoutOrReturn);
				m_PopOutButton->SetHidden(false);
			}

			void Render(Skin::Base* skin) override
			{
				skin->DrawTabTitleBar(this);
			}

			void DragAndDrop_StartDragging(Gwen::DragAndDrop::Package* pPackage, int x, int y) override
			{
				DragAndDrop::SourceControl = GetParent();
				DragAndDrop::SourceControl->DragAndDrop_StartDragging(pPackage, x, y);
			}

			void UpdateFromTab(TabButton* pButton, bool popped_out)
			{
				SetText(pButton->GetText());
				SizeToContents();
				m_CurrentButton = pButton;
				m_CloseButton->SetHidden(!pButton->IsClosable());
				m_PopOutButton->SetText(popped_out ? "\\/" : "^");
				m_PopOutIsReturn = popped_out;
				m_PopOutButton->SetHidden(!pButton->IsPopoutable());
			}

		private:
			
			void OnPopoutOrReturn(Controls::Base* control);
			void OnCloseTab(Controls::Base* control);
			
			TabButton* m_CurrentButton;
			Button* m_CloseButton;
			Button* m_PopOutButton;
			bool m_PopOutIsReturn;
		};
	}
}
#endif
