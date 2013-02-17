/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_TEXTOBJECT_H
#define GWEN_TEXTOBJECT_H

#include "Gwen/Utility.h"

namespace Gwen
{
	/*

		TextObjects can be either a UnicodeString or a String

		Just makes things easier instead of having a function taking both.

	*/
	class TextObject
	{
		public:

			TextObject() {}

			TextObject( const Gwen::String & text )
			{
				m_String = text;
				m_Unicode = Gwen::Utility::StringToUnicode( m_String );
			}

			TextObject( const char* text )
			{
				m_String = text;
				m_Unicode = Gwen::Utility::StringToUnicode( m_String );
			}

			TextObject( const wchar_t* text )
			{
				m_Unicode = text;
				m_String = Gwen::Utility::UnicodeToString( m_Unicode );
			}

			TextObject( const Gwen::UnicodeString & unicode )
			{
				*this = unicode;
			}

			operator const Gwen::String & () { return m_String; }
			operator const Gwen::UnicodeString & () { return m_Unicode; }

			void operator = ( const char* str )
			{
				m_String = str;
				m_Unicode = Gwen::Utility::StringToUnicode( m_String );
			}

			void operator = ( const Gwen::String & str )
			{
				m_String = str;
				m_Unicode = Gwen::Utility::StringToUnicode( m_String );
			}

			void operator = ( const Gwen::UnicodeString & unicodeStr )
			{
				m_Unicode = unicodeStr;
				m_String = Gwen::Utility::UnicodeToString( m_Unicode );
			}

			bool operator == ( const TextObject & to ) const
			{
				return m_Unicode == to.m_Unicode;
			}

			const Gwen::String & Get() const
			{
				return m_String;
			}

			const char* c_str() const
			{
				return m_String.c_str();
			}

			const Gwen::UnicodeString & GetUnicode() const
			{
				return m_Unicode;
			}

			int length() const { return m_Unicode.length(); }

			Gwen::UnicodeString		m_Unicode;
			Gwen::String			m_String;
	};
}
#endif
