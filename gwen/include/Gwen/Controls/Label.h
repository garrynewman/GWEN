/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_LABEL_H
#define GWEN_CONTROLS_LABEL_H

#include "Gwen/BaseRender.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Text.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT Label : public Controls::Base
		{
			public:

				GWEN_CONTROL( Label, Controls::Base );
				virtual void PreDelete( Gwen::Skin::Base* skin ) override;

				virtual void SetText( const TextObject & str, bool bDoEvents = true );

				virtual const TextObject & GetText() const { return m_Text->GetText(); }

				virtual void Render( Skin::Base* /*skin*/ ) override {}

				virtual void PostLayout( Skin::Base* skin ) override;

				virtual void SizeToContents();

				void SetAlignment( int iAlign );
				int GetAlignment();

				void SetFont( Gwen::UnicodeString strFacename, int iSize, bool bBold );
				void SetFont( Gwen::Font* pFont ) { m_Text->SetFont( pFont ); }
				Gwen::Font* GetFont() { return m_Text->GetFont(); }

				void SetTextColor( const Gwen::Color & col ) { m_Text->SetTextColor( col ); }
				void SetTextColorOverride( const Gwen::Color & col ) { m_Text->SetTextColorOverride( col ); }
				inline const Gwen::Color & TextColor() const { return m_Text->TextColor(); }

				int TextWidth() { return m_Text->Width(); }
				int TextRight() { return m_Text->Right(); }
				int TextHeight() { return m_Text->Height(); }
				int TextX() { return m_Text->X(); }
				int TextY() { return m_Text->Y(); }
				int TextLength() { return m_Text->Length(); }

				Gwen::Rect GetCharacterPosition( int iChar );

				void SetTextPadding( const Padding & padding ) { m_Text->SetPadding( padding ); Invalidate(); InvalidateParent(); }
				const Padding & GetTextPadding() { return m_Text->GetPadding(); }

				virtual TextObject GetValue() override { return GetText(); }
				virtual void SetValue( const TextObject & strValue ) override { return SetText( strValue ); }

				virtual bool Wrap() { return m_Text->Wrap(); }
				virtual void SetWrap( bool b ) { m_Text->SetWrap( b ); }

				virtual void OnBoundsChanged( Gwen::Rect oldChildBounds ) override;

			protected:

				virtual void OnTextChanged() {};

				Gwen::Font*					m_CreatedFont;
				ControlsInternal::Text*		m_Text;
				int m_iAlign;


		};
	}
}
#endif
