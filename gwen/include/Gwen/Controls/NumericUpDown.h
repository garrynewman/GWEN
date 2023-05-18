/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_NUMERICUPDOWN_H
#define GWEN_CONTROLS_NUMERICUPDOWN_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/TextBox.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT NumericUpDownButton_Up : public Button
		{
				GWEN_CONTROL_INLINE( NumericUpDownButton_Up, Button )
				{
					SetSize( 7, 7 );
				}

				virtual void Render( Skin::Base* skin ) override
				{
					skin->DrawNumericUpDownButton( this, IsDepressed(), true, IsHovered() );
				}

				virtual bool ShouldRedrawOnHover() override { return true; }
		};

		class GWEN_EXPORT NumericUpDownButton_Down : public Button
		{
				GWEN_CONTROL_INLINE( NumericUpDownButton_Down, Button )
				{
					SetSize( 7, 7 );
				}

				virtual void Render( Skin::Base* skin ) override
				{
					skin->DrawNumericUpDownButton( this, IsDepressed(), false, IsHovered() );
				}

				virtual bool ShouldRedrawOnHover() override { return true; }
		};

		class GWEN_EXPORT NumericUpDown : public TextBoxNumeric
		{
			public:

				GWEN_CONTROL( NumericUpDown, TextBoxNumeric );

				void SetMin( int i );
				void SetMax( int i );
				void SetValue( int i, bool force = true );
				void SetIncrement( int i ) { m_iIncrement = i; }
				int GetValueNumeric() { return m_iNumber; }

				Event::Caller	onChanged;

			private:

				void OnEnter();
				void OnChange();
				void OnTextChanged();

				void OnButtonUp( Base* control );
				void OnButtonDown( Base* control );

				virtual bool OnKeyUp( bool bDown ) override	{	if ( bDown ) { OnButtonUp( NULL ); } return true;   }
				virtual bool OnKeyDown( bool bDown ) override {	if ( bDown ) { OnButtonDown( NULL ); } return true; }

				virtual void SyncTextFromNumber();
				virtual void SyncNumberFromText();

				virtual void OnLostKeyboardFocus();


				int m_iNumber;
				int m_iMax;
				int m_iMin;
				int m_iIncrement;
		};
		
		class GWEN_EXPORT FloatUpDown : public TextBoxNumeric
		{
			public:

				GWEN_CONTROL( FloatUpDown, TextBoxNumeric );

				void SetMin( double i );
				void SetMax( double i );
				void SetValue( double i, bool force = true );
				void SetIncrement( double i ) { m_iIncrement = i; }
				double GetValueNumeric() { return m_iNumber; }

				Event::Caller	onChanged;

			private:

				void OnEnter();
				void OnChange();
				void OnTextChanged();

				void OnButtonUp( Base* control );
				void OnButtonDown( Base* control );

				virtual bool OnKeyUp( bool bDown ) override	{	if ( bDown ) { OnButtonUp( NULL ); } return true;   }
				virtual bool OnKeyDown( bool bDown ) override {	if ( bDown ) { OnButtonDown( NULL ); } return true; }

				virtual void SyncTextFromNumber();
				virtual void SyncNumberFromText();

				virtual void OnLostKeyboardFocus();


				double m_iNumber;
				double m_iMax;
				double m_iMin;
				double m_iIncrement;
		};
	}
}
#endif
