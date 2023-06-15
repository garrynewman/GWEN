/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_TEXTBOX_H
#define GWEN_CONTROLS_TEXTOBX_H

#include "Gwen/BaseRender.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/ScrollControl.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT TextBox : public Label
		{
				GWEN_CONTROL( TextBox, Label );

				virtual void Render( Skin::Base* skin ) override;
				virtual void RenderFocus( Gwen::Skin::Base* /*skin*/ ) override {};
				virtual void Layout( Skin::Base* skin ) override;
				virtual void PostLayout( Skin::Base* skin ) override;

#ifndef GWEN_NO_ANIMATION
				virtual void UpdateCaretColor();
#endif

				virtual bool OnChar( Gwen::UnicodeChar c );

				virtual void InsertText( const Gwen::UnicodeString & str );
				virtual void DeleteText( int iStartPos, int iLength );

				virtual void SetPlaceholderText(const TextObject& text) { m_PlaceholderText = text; }
				virtual void SetMaxLength( int length ) { m_iMaxLength = length; }

				virtual void RefreshCursorBounds();

				virtual bool OnKeyReturn( bool bDown ) override;
				virtual bool OnKeyBackspace( bool bDown ) override;
				virtual bool OnKeyDelete( bool bDown ) override;
				virtual bool OnKeyRight( bool bDown ) override;
				virtual bool OnKeyLeft( bool bDown ) override;
				virtual bool OnKeyHome( bool bDown ) override;
				virtual bool OnKeyEnd( bool bDown ) override;

				virtual bool AccelOnlyFocus() override { return true; }

				virtual void OnPaste( Gwen::Controls::Base* pCtrl ) override;
				virtual void OnCopy( Gwen::Controls::Base* pCtrl ) override;
				virtual void OnCut( Gwen::Controls::Base* pCtrl ) override;
				virtual void OnSelectAll( Gwen::Controls::Base* pCtrl ) override;

				virtual void OnMouseDoubleClickLeft( int x, int y ) override;

				virtual void EraseSelection();
				virtual bool HasSelection();
				virtual UnicodeString GetSelection();

				virtual void SetCursorPos( int i );
				virtual void SetCursorEnd( int i );

				virtual void OnMouseClickLeft( int x, int y, bool bDown ) override;
				virtual void OnMouseClickRight( int /*x*/, int /*y*/, bool /*bDown*/ ) override;
				virtual void OnMouseMoved( int x, int y, int deltaX, int deltaY ) override;

                virtual void SetEditable( bool b ) { m_bEditable = b; }

				virtual void SetSelectAllOnFocus( bool b ) { m_bSelectAll = b; if ( b ) { OnSelectAll( this ); } }

				virtual void MakeCaratVisible();

				virtual void OnEnter();
				
				virtual void OnKeyboardFocus() override;
				virtual void OnLostKeyboardFocus() override;

				virtual bool NeedsInputChars() override { return true; }

				virtual void MoveCaretToEnd();
				virtual void MoveCaretToStart();

				void EnableContextMenu(bool enable) { m_bEnableContextMenu = enable; }

				Event::Caller	onTextChanged;
				Event::Caller	onReturnPressed;
				Event::Caller   onFocusGained;
				Event::Caller	onFocusLost;

			protected:

				virtual void OnTextChanged();
				virtual bool IsTextAllowed( const Gwen::UnicodeString & /*str*/, int /*iPos*/ ) { return true; }

                bool m_bEditable;
				bool m_bSelectAll;
				bool m_bEnableContextMenu;

				int m_iCursorPos;
				int m_iCursorEnd;
				int m_iCursorLine;

				int m_iMaxLength;

				double m_dLastClickTime;

				Gwen::TextObject m_PlaceholderText;

				Gwen::Rect m_rectSelectionBounds;
				Gwen::Rect m_rectCaretBounds;

				float m_fNextCaretColorChange;
				Gwen::Color	m_CaretColor;
		};

		class GWEN_EXPORT TextBoxNumeric : public TextBox
		{
			public:

				GWEN_CONTROL( TextBoxNumeric, TextBox );

				virtual float GetFloatFromText();

			private:

				virtual bool IsTextAllowed( const Gwen::UnicodeString & str, int iPos ) override;

		};

		class GWEN_EXPORT TextBoxMultiline : public TextBox
		{
			public:

				GWEN_CONTROL( TextBoxMultiline, TextBox );

				virtual bool OnKeyReturn( bool bDown ) override;
				virtual void Render( Skin::Base* skin ) override;
				virtual void MakeCaratVisible() override;

				virtual bool OnKeyHome( bool bDown ) override;
				virtual bool OnKeyEnd( bool bDown ) override;
				virtual bool OnKeyUp( bool bDown ) override;
				virtual bool OnKeyDown( bool bDown ) override;

				virtual int GetCurrentLine();

			protected:

		};

		class GWEN_EXPORT PasswordTextBox : public TextBox
		{
			public:

				GWEN_CONTROL( PasswordTextBox, TextBox );

				virtual void SetText( const TextObject& str, bool bDoEvents = true ) override;
				virtual void SetPasswordChar(const char c);

				virtual const TextObject& GetText() const override { return m_realText; }
			protected:

				char m_passwordChar;
				TextObject	 m_realText;
		};
	}
}
#endif
