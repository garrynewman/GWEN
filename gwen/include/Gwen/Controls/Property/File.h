
#pragma once
#ifndef GWEN_CONTROLS_PROPERTY_FILE_H
#define GWEN_CONTROLS_PROPERTY_FILE_H

#include "Gwen/Controls/Properties.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Dialogs/FolderOpen.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Property
		{
			class File : public Property::Text
			{
					GWEN_CONTROL_INLINE( File, Property::Text )
					{
						Controls::Button* pButton = new Controls::Button( this );
						pButton->Dock( Pos::Right );
						pButton->SetText( "..." );
						pButton->SetWidth( 20 );
						pButton->onPress.Add( this, &ThisClass::OnButtonPress );
						pButton->SetMargin( Margin( 1, 1, 1, 2 ) );
						m_strDialogName = "Find File";
						m_strFileExtension = "*.*";
					}

					File* SetExtension( const Gwen::String & string )
					{
						m_strFileExtension = string;
						return this;
					}

					void OnButtonPress( Controls::Base* control )
					{
						Gwen::Dialogs::FileOpen( true, m_strDialogName, m_TextBox->GetText().Get(), m_strFileExtension, this, &File::EventFilePicked );
					}

					void EventFilePicked( Event::Info info )
					{
						m_TextBox->SetText( info.String );
					}


					String	m_strDialogName;
					String	m_strFileExtension;

			};
		}
	}
}
#endif
