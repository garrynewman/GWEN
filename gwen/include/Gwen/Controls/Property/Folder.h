
#pragma once
#ifndef GWEN_CONTROLS_PROPERTY_FOLDER_H
#define GWEN_CONTROLS_PROPERTY_FOLDER_H

#include "Gwen/Controls/Properties.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Dialogs/FolderOpen.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Property
		{
			class Folder : public Property::Text
			{
					GWEN_CONTROL_INLINE( Folder, Property::Text )
					{
						Controls::Button* pButton = new Controls::Button( this );
						pButton->Dock( Pos::Right );
						pButton->SetText( "..." );
						pButton->SetWidth( 20 );
						pButton->onPress.Add( this, &ThisClass::OnButtonPress );
						pButton->SetMargin( Margin( 1, 1, 1, 2 ) );
						m_strDialogName = "Find Folder";
					}

					void OnButtonPress( Controls::Base* control )
					{
						Gwen::Dialogs::FolderOpen( true, m_strDialogName, m_TextBox->GetText().Get(), this, &Folder::EventFilePicked );
					}

					void EventFilePicked( Event::Info info )
					{
						m_TextBox->SetText( info.String );
					}


					String	m_strDialogName;

			};
		}
	}
}
#endif
