/*
	GWEN
	Copyright (c) 2022 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_PROPERTY_BUTTON_H
#define GWEN_CONTROLS_PROPERTY_BUTTON_H

#include "Gwen/Controls/Properties.h"
#include "Gwen/Controls/Button.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Property
		{
			class Button : public Property::Base
			{
					GWEN_CONTROL_INLINE( Button, Property::Base )
					{
						Controls::Button* pButton = new Controls::Button( this );
						pButton->Dock( Pos::Fill );
						pButton->SetText( "..." );
						pButton->onPress.Add( this, &ThisClass::OnButtonPress );
						pButton->SetMargin( Margin( 1, 1, 1, 2 ) );
						m_pButton = pButton;
					}
					
					virtual void SetPropertyValue( const TextObject & v, bool bFireChangeEvent = false)
					{
						m_pButton->SetText(v);
					}
					
					virtual TextObject GetPropertyValue()
					{
						return "";
					}
					
					virtual bool IsEditing()
					{
						return false;
					}

					void OnButtonPress( Controls::Base* control )
					{
						DoChanged();
					}
				private:
				
					Controls::Button* m_pButton;
			};
		}
	}
}
#endif
