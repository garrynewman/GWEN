/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_PROPERTY_COMBOBOX_H
#define GWEN_CONTROLS_PROPERTY_COMBOBOX_H

#include "Gwen/Controls/Property/BaseProperty.h"
#include "Gwen/Controls/ComboBox.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Property
		{
			class GWEN_EXPORT ComboBox : public Property::Base
			{
				public:

					GWEN_CONTROL_INLINE( ComboBox, Property::Base )
					{
						m_ComboBox = new Gwen::Controls::ComboBox( this );
						m_ComboBox->Dock( Pos::Fill );
						m_ComboBox->onSelection.Add( this, &BaseClass::OnPropertyValueChanged );
						m_ComboBox->SetTabable( true );
						m_ComboBox->SetKeyboardInputEnabled( true );
						m_ComboBox->SetShouldDrawBackground( false );
						SetHeight( 18 );
					}

					virtual TextObject GetPropertyValue()
					{
						Gwen::Controls::Label* pControl = m_ComboBox->GetSelectedItem();

						if ( !pControl ) { return L""; }

						return pControl->GetName();
					}

					virtual void SetPropertyValue( const TextObject & v, bool bFireChangeEvents )
					{
						m_ComboBox->SelectItemByName( v.Get(), bFireChangeEvents );
					}

					virtual bool IsEditing()
					{
						return m_ComboBox->HasFocus();
					}

					virtual bool IsHovered()
					{
						return BaseClass::IsHovered() || m_ComboBox->IsHovered();
					}

					Gwen::Controls::ComboBox* GetComboBox()
					{
						return m_ComboBox;
					}

				protected:

					Gwen::Controls::ComboBox* m_ComboBox;
			};
		}
	}
}
#endif
