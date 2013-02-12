/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_TEXT_H
#define GWEN_CONTROLS_TEXT_H

#include "Gwen/BaseRender.h"
#include "Gwen/Controls/Base.h"

namespace Gwen 
{
	namespace ControlsInternal
	{
		class GWEN_EXPORT Text : public Controls::Base
		{
			public:

				GWEN_CONTROL( Text, Controls::Base );

				virtual ~Text();
				Gwen::Font* GetFont();

				void SetString( const TextObject& str );
				
				void Render( Skin::Base* skin );
				void Layout( Skin::Base* skin );

				void RefreshSize();

				void SetFont( Gwen::Font* pFont );

				const TextObject& GetText() const { return m_String; }

				Gwen::Rect GetCharacterPosition( int iChar );
				int GetClosestCharacter( Gwen::Point p );
				
				/// return the encapsulating box of the given line
				// @remark this function is necessary to implement multiline selection
				virtual Gwen::Rect GetLineBox( int i );

				int Length() const { return (int)m_String.GetUnicode().size(); }

				virtual void SetTextColor( const Gwen::Color& col ){ m_Color = col; }
				virtual void SetTextColorOverride( const Gwen::Color& col ){ m_ColorOverride = col; }

				virtual void OnScaleChanged();

				inline const Gwen::Color &TextColor() const { return m_Color; }

				virtual void TextChanged(){ m_bTextChanged = true; }
				virtual bool Wrap(){ return m_bWrap; }
				virtual void SetWrap( bool b ){ if ( m_bWrap == b ) return; m_bWrap = b; m_bTextChanged = true; Invalidate(); }
				
				/// return a line by it's index
				virtual Text* GetLine( int i );
				/// return the line index of the line containing the character in the global string
				virtual int GetLineFromChar( int i );
				/// return the global character id of the first character of the line
				virtual int GetStartCharFromLine( int i );
				/// return the global character id of the last character of the line
				/// @remark in a multiline context the last character is the first character of the next line !
				virtual int GetEndCharFromLine( int i );
				/// return the relative character index of a global character index inside the line
				virtual int GetCharPosOnLine( int i );

				virtual int NumLines();
			protected:
				virtual void SplitWords(const Gwen::UnicodeString &s, std::vector<Gwen::UnicodeString> &elems);
			private:
				
				virtual void RefreshSizeWrap();

				Gwen::TextObject	m_String;
				Gwen::Font*			m_Font;
				Gwen::Color			m_Color;
				Gwen::Color			m_ColorOverride;

				bool				m_bWrap;
				bool				m_bTextChanged;

				typedef std::list<Text*> TextLines;
				TextLines		m_Lines;
		};
	}

}
#endif
