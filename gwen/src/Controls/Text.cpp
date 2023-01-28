/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Controls/Text.h"
#include "Gwen/Skin.h"
#include "Gwen/Utility.h"

using namespace Gwen;
using namespace Gwen::ControlsInternal;

GWEN_CONTROL_CONSTRUCTOR( Text )
{
	m_Font = NULL;
	m_ColorOverride = Color( 255, 255, 255, 0 );
	m_Color = GetSkin()->Colors.Label.Default;
	m_bWrap = false;
	SetMouseInputEnabled( false );
	SetWrap( false );
}

Text::~Text()
{
	// NOTE: This font doesn't need to be released
	// Because it's a pointer to another font somewhere.
}

void Text::Layout( Skin::Base* skin )
{
	if ( m_bTextChanged )
	{
		RefreshSize();
		m_bTextChanged = false;
	}
}

Gwen::Font* Text::GetFont()
{
	return m_Font;
}

void Text::SetFont( Gwen::Font* pFont )
{
	if ( m_Font == pFont ) { return; }

	m_Font = pFont;
	m_bTextChanged = true;
	// Change the font of multilines too!
	{
		TextLines::iterator it = m_Lines.begin();
		TextLines::iterator itEnd = m_Lines.end();

		while ( it != itEnd )
		{
			( *it )->SetFont( m_Font );
			++it;
		}
	}
	Invalidate();
}

void Text::SetString( const TextObject & str )
{
	if ( m_String == str ) { return; }

	m_String = str.GetUnicode();
	m_bTextChanged = true;
	Invalidate();
}

void Text::Render( Skin::Base* skin )
{
	if ( m_bWrap ) { return; }

	if ( Length() == 0 || !GetFont() ) { return; }

	if ( m_ColorOverride.a == 0 )
	{ skin->GetRender()->SetDrawColor( m_Color ); }
	else
	{ skin->GetRender()->SetDrawColor( m_ColorOverride ); }

	skin->GetRender()->RenderText( GetFont(), Gwen::PointF( GetPadding().left, GetPadding().top ), m_String );
}

Gwen::Rect Text::GetCharacterPosition( int iChar )
{
	if ( !m_Lines.empty() )
	{
		TextLines::iterator it = m_Lines.begin();
		TextLines::iterator itEnd = m_Lines.end();
		int iChars = 0;

		Text* pLine;
		while ( it != itEnd )
		{
			pLine = *it;
			++it;
			iChars += pLine->Length();

			if ( iChars <= iChar ) { continue; }

			iChars -= pLine->Length();
			Gwen::Rect rect = pLine->GetCharacterPosition( iChar - iChars );
			rect.x += pLine->X();
			rect.y += pLine->Y();
			return rect;
		}
		//manage special case of the last character
		Gwen::Rect rect = pLine->GetCharacterPosition( pLine->Length() );
		rect.x += pLine->X();
		rect.y += pLine->Y();
		return rect; 
	}

	if ( Length() == 0 || iChar == 0 )
	{
		Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), Gwen::UnicodeString(L" ") );
		return Gwen::Rect( 0, 0, 0, p.y );
	}

	UnicodeString sub = m_String.GetUnicode().substr( 0, iChar );
	Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), sub );
	return Rect( p.x, 0, 0, p.y );
}

Gwen::Rect Text::GetLineBox( int i )
{
	Text* line = GetLine(i);
	if (line != NULL)
	{
		Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), line->m_String);
		return Gwen::Rect(line->X(), line->Y(), Clamp(p.x, 1.0f,p.x), Clamp(p.y, 1.0f,p.y) );
	}
	else
	{
		Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), m_String);
		return Gwen::Rect(0, 0, Clamp(p.x, 1.0f,p.x), Clamp(p.y, 1.0f,p.y) );
	}
}
 

int Text::GetClosestCharacter( Gwen::Point p )
{
	if ( !m_Lines.empty() )
	{
		TextLines::iterator it = m_Lines.begin();
		TextLines::iterator itEnd = m_Lines.end();
		int iChars = 0;

		Text* pLine;
		while ( it != itEnd )
		{
			pLine = *it;
			++it;
			iChars += pLine->Length();

			if ( p.y < pLine->Y() ) { continue; }

			if ( p.y > pLine->Bottom() ) { continue; }

			if ( p.y < pLine->Bottom()) break;
		}
		iChars -= pLine->Length();
		int iLinePos = pLine->GetClosestCharacter( Gwen::Point( p.x - pLine->X(), p.y - pLine->Y() ) );
		return iChars + iLinePos;
	}

	int iDistance = 4096;
	int iChar = 0;

	for ( size_t i = 0; i < m_String.GetUnicode().length() + 1; i++ )
	{
		Gwen::Rect cp = GetCharacterPosition( i );
		int iDist = abs( cp.x - p.x ) + abs( cp.y - p.y );   // this isn't proper

		if ( iDist > iDistance ) { continue; }

		iDistance = iDist;
		iChar = i;
	}

	return iChar;
}

void Text::OnScaleChanged()
{
	m_bTextChanged = true;
	Invalidate();
}

void Text::RefreshSize()
{
	if ( m_bWrap )
	{
		return RefreshSizeWrap();
	}

	if ( !GetFont() )
	{
		Debug::AssertCheck( 0, "Text::RefreshSize() - No Font!!\n" );
		return;
	}

	Gwen::PointF p( 1, GetFont()->size );

	if ( Length() > 0 )
	{
		p = GetSkin()->GetRender()->MeasureText( GetFont(), m_String.GetUnicode() );
	}

	p.x += GetPadding().left + GetPadding().right;
	p.y += GetPadding().top + GetPadding().bottom;

	if ( p.x == Width() && p.y == Height() )
	{ return; }

	if ( p.y < GetFont()->size ) { p.y = GetFont()->size; }

	SetSize( p.x, p.y );
	InvalidateParent();
	Invalidate();
}

void Text::SplitWords(const Gwen::UnicodeString &s, std::vector<Gwen::UnicodeString> & elems )
{
	Gwen::UnicodeString str;

	int w = GetParent()->Width() - GetParent()->GetPadding().left-GetParent()->GetPadding().right;
	for ( int i=0; i<(int)s.length(); i++ ) 
	{
		if ( s[i] == L'\n' )
		{
			if ( !str.empty() ) { elems.push_back( str ); }

			elems.push_back( L"\n" );
			str.clear();
			continue;
		}

		if ( s[i] == L' ' )
		{
			str += s[i];
			elems.push_back( str );
			str.clear();
			continue;
		}

		str += s[i];

		//if adding character makes the word bigger than the textbox size
		Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), str );
		if (str.length() == 1 && p.x > w)
		{
			return;// give up since we are too narrow
		}

		if ( p.x > w ) 
		{
			int addSum = GetPadding().left+GetPadding().right;
			//split words
			str.resize( str.size() - 1 );
			elems.push_back( str );
			str.clear();
			--i;
			continue;
		}
	}

	if ( !str.empty() ) { elems.push_back( str ); }
}

void Text::RefreshSizeWrap()
{
	DeleteAllChildren();

	for ( TextLines::iterator it = m_Lines.begin(); it != m_Lines.end(); ++it )
	{
		delete *it;
	}

	m_Lines.clear();
	std::vector<Gwen::UnicodeString> words;
	SplitWords( GetText().GetUnicode(), words );
	// Adding a bullshit word to the end simplifies the code below
	// which is anything but simple.
	words.push_back( L"" );

	if ( !GetFont() )
	{
		Debug::AssertCheck( 0, "Text::RefreshSize() - No Font!!\n" );
		return;
	}

	PointF pFontSize = GetSkin()->GetRender()->MeasureText( GetFont(), Gwen::UnicodeString(L" ") );
	int w = GetParent()->Width() - GetParent()->GetPadding().left-GetParent()->GetPadding().right; 
	int x = 0, y = 0;
	Gwen::UnicodeString strLine;

	for ( std::vector<Gwen::UnicodeString>::iterator it = words.begin(); it != words.end(); ++it )
	{
		bool bFinishLine = false;
		bool bWrapped = false;

		// If this word is a newline - make a newline (we still add it to the text)
		if ( ( *it ).c_str() [0] == L'\n' ) { bFinishLine = true; }

		// Does adding this word drive us over the width?
		{
			strLine += ( *it );
			Gwen::PointF p = GetSkin()->GetRender()->MeasureText( GetFont(), strLine );

			if ( p.x > w ) { bFinishLine = true; bWrapped = true; }
		}

		// If this is the last word then finish the line
		if ( --words.end() == it )
		{
			bFinishLine = true;
		}

		if ( bFinishLine )
		{
			Text* t = new Text( this );
			t->SetFont( GetFont() );
			if(bWrapped)
			{
				t->SetString( strLine.substr( 0, strLine.length() - (*it).length() ) );
				// newline should start with the word that was too big
				strLine = *it;
			}
			else
			{
				t->SetString( strLine.substr( 0, strLine.length()) );
				//new line is empty
				strLine.clear();
			} 
			t->RefreshSize();
			t->SetPos( x, y );
			m_Lines.push_back( t );

			// newline should start with the word that was too big
			// strLine = *it;

			// Position the newline
			y += pFontSize.y;
			x = 0;
			//if ( strLine[0] == L' ' ) x -= pFontSize.x;
		}
	}

	// Size to children height and parent width
	{
		Point childsize = ChildrenSize();
		SetSize( w, childsize.y );
	}
	InvalidateParent();
	Invalidate();
}

int Text::NumLines()
{
	return m_Lines.size();
}

Text* Text::GetLine( int i )
{
	TextLines::iterator it = m_Lines.begin();
	TextLines::iterator itEnd = m_Lines.end();

	while ( it != itEnd )
	{
		if ( i == 0 ) { return *it; }

		++it;
		i--;
	}

	return NULL;
}

int Text::GetLineFromChar( int i )
{
	TextLines::iterator it = m_Lines.begin();
	TextLines::iterator itEnd = m_Lines.end();
	int iChars = 0;
	int iLine = 0;

	while ( it != itEnd )
	{
		Text* pLine = *it;
		++it;
		iChars += pLine->Length();

		if ( iChars > i ) { return iLine; }

		iLine++;
	}

	if(iLine>0) return iLine-1;
	return iLine;
}

int Text::GetStartCharFromLine( int i )
{
	TextLines::iterator it = m_Lines.begin();
	TextLines::iterator itEnd = m_Lines.end();
	int iChars = 0;

	while ( it != itEnd )
	{
		Text* pLine = *it;
		++it;

		if ( i == 0 ) { return Gwen::Clamp( iChars, 0, Length() ); }

		iChars += pLine->Length();
		i--;
	}

	return Gwen::Clamp( iChars, 0, Length() );
}

int Text::GetEndCharFromLine( int i )
{
	int iStart = GetStartCharFromLine( i );
	Text* iLine = GetLine( i );

	if ( iLine )
	{
		iStart += iLine->Length();
	}

	return Gwen::Clamp( iStart, 0, Length() );
}

int Text::GetCharPosOnLine( int i )
{
	int iLine = GetLineFromChar( i );
	Text* line = GetLine( iLine );

	if ( !line ) { return 0; }

	int iStart = GetStartCharFromLine( iLine );
	return i - iStart;
}
