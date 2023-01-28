/*
	GWEN
	Copyright (c) 2022 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_PROPERTY_NUMERIC_H
#define GWEN_CONTROLS_PROPERTY_NUMERIC_H

#include "Gwen/Controls/Property/BaseProperty.h"
#include "Gwen/Controls/NumericUpDown.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Property
		{
			class GWEN_EXPORT Numeric : public Property::Base
			{
				public:

					GWEN_CONTROL_INLINE( Numeric, Property::Base )
					{
						m_Numeric = new Controls::NumericUpDown( this );
						m_Numeric->SetValue( 50 );
						m_Numeric->SetMax( 1000 );
						m_Numeric->SetMin( -1000 );
						m_Numeric->Dock( Pos::Fill );
						//m_Numeric->SetShouldDrawBackground( false );
						m_Numeric->onChanged.Add( this, &BaseClass::OnPropertyValueChanged );
						m_Numeric->onHoverEnter.Add( this, &ThisClass::OnPropertyHoverChanged );
						m_Numeric->onHoverLeave.Add( this, &ThisClass::OnPropertyHoverChanged );
					}

					virtual TextObject GetPropertyValue()
					{
						return Utility::ToString(m_Numeric->GetValueNumeric());
					}

					virtual void SetPropertyValue( const TextObject & v, bool bFireChangeEvents )
					{
						m_Numeric->SetText( v, bFireChangeEvents );
					}

					virtual bool IsEditing()
					{
						return m_Numeric->HasFocus();
					}

					virtual bool IsHovered()
					{
						return BaseClass::IsHovered() || m_Numeric->IsHovered();
					}

					NumericUpDown* m_Numeric;

				private:

					void OnPropertyHoverChanged( Gwen::Controls::Base* control )
					{
						Redraw();
					}
			};
			
			class GWEN_EXPORT Float : public Property::Base
			{
				public:

					GWEN_CONTROL_INLINE( Float, Property::Base )
					{
						m_Numeric = new Controls::FloatUpDown( this );
						m_Numeric->SetValue( 50 );
						m_Numeric->SetMax( 1000 );
						m_Numeric->SetMin( -1000 );
						m_Numeric->Dock( Pos::Fill );
						//m_Numeric->SetShouldDrawBackground( false );
						m_Numeric->onChanged.Add( this, &BaseClass::OnPropertyValueChanged );
					}

					virtual TextObject GetPropertyValue()
					{
						return Utility::ToString(m_Numeric->GetValueNumeric());
					}

					virtual void SetPropertyValue( const TextObject & v, bool bFireChangeEvents )
					{
						m_Numeric->SetText( v, bFireChangeEvents );
					}

					virtual bool IsEditing()
					{
						return m_Numeric->HasFocus();
					}

					virtual bool IsHovered()
					{
						return BaseClass::IsHovered() || m_Numeric->IsHovered();
					}

					FloatUpDown* m_Numeric;
			};
		}
	}
}
#endif
