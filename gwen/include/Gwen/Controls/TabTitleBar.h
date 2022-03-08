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
#include "Gwen/Controls/WindowControl.h"
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
				m_CloseButton->SetHidden(false);
				
				m_PopOutButton = new Controls::Button(this);
				m_PopOutButton->SetSize(20, 20);
				m_PopOutButton->SetPos(0, 2);
				m_PopOutButton->Dock(Pos::Right);
				m_PopOutButton->SetText("^");
				m_PopOutButton->onPress.Add(this, &ThisClass::OnPopout);
				m_PopOutButton->SetHidden(false);
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
				m_PopOutButton->SetHidden(!pButton->IsClosable());
			}

		private:
		
			struct ReturnButtonData
			{
				DockBase* dock;// original root dock this was a part of
				Base* inner;// what is held in this panel
				bool closable;
				std::string title;
				WindowControl* window;// our parent window
			};
			
			void OnPopout(Controls::Base* control)
			{
				if (!m_CurrentButton)
				{
					return;
				}
				
				// first, lets iterate up to find the topmost dockbase
				//okay, we need to find the dockbase we came from so we can unpin later
				//also preferrably find the side we were pinned to
				
				TabControl* tab = m_CurrentButton->GetTabControl();
				// now go up until we find a DockBase
				Base* current = tab;
				DockBase* dock = dynamic_cast<DockBase*>(current);
				while (current && dock == 0)
				{
					current = current->GetParent();
					dock = dynamic_cast<DockBase*>(current);
				}
				if (dock)
				{
					// now go to the top dock base
					while (dynamic_cast<DockBase*>(dock->GetParent()) != 0)
					{
						dock = dynamic_cast<DockBase*>(dock->GetParent());
					}
				}
				else
				{
					return;
				}
				
				bool closable = m_CurrentButton->IsClosable();
				
				auto page = m_CurrentButton->GetPage();
				m_CurrentButton->GetTabControl()->RemovePage(m_CurrentButton);
				page->DelayedDelete();
				m_CurrentButton->DelayedDelete();
				
				auto inner = page->GetChildren().front();
				
				Controls::WindowControl* pWindow = new Controls::WindowControl( GetCanvas() );
				pWindow->SetTitle( m_CurrentButton->GetText().Get() );
				pWindow->SetSize( inner->Width(), inner->Height() );
				pWindow->SetPos( inner->LocalPosToCanvas(inner->GetPos()) );
				pWindow->SetDeleteOnClose( true );
				inner->SetParent(pWindow);
				
				//now add a button that lets us go back to the dock
				auto button = new Controls::Button(pWindow);
				button->SetSize(20, 20);
				button->SetPos(0, 2);
				button->Dock(Pos::Right);
				button->SetSize(20, 20);
				button->SetText("^");
				ReturnButtonData* data = new ReturnButtonData;
				data->title = m_CurrentButton->GetText().Get();
				data->closable = closable;
				data->inner = inner;
				data->window = pWindow;
				data->dock = dock;
				button->onPress.Add(this, &ThisClass::OnReturn, data);
				button->SetHidden(false);
				
				m_CurrentButton = 0;
			}
			
			void OnReturn(Gwen::Event::Info info)
			{
				ReturnButtonData* data = (ReturnButtonData*)info.Data;
				auto button = data->dock->GetRight()->GetTabControl()->AddPage(data->title);
				button->SetClosable(data->closable);
				auto page = button->GetPage();
				data->inner->SetParent(page);
				info.Control->Dock(Pos::Fill);
				
				data->window->CloseButtonPressed();
				
				delete data;
			}

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
			Button* m_PopOutButton;
		};
	}
}
#endif
