/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/ToolTip.h"
#include "Gwen/Utility.h"

#include <cstdio>

using namespace Gwen;

#ifdef _MSC_VER
#pragma warning(disable:4267)// conversion from 'size_t' to 'int', possible loss of data
#endif

#ifdef __MINGW32__
#undef vswprintf
#define vswprintf _vsnwprintf
#endif

#ifdef _MSC_VER
#define GWEN_FNULL "NUL"
#define va_copy(d,s) ((d) = (s))
#else
#define GWEN_FNULL "/dev/null"
#endif

using convert_type = std::codecvt_utf8<wchar_t>;
namespace Gwen
{
	namespace Utility
	{
		std::wstring_convert<convert_type, wchar_t> wstring_converter;
	}
}

UnicodeString Gwen::Utility::Format( const wchar_t* fmt, ... )
{
	va_list s;
	int len = 0;
	
	va_start( s, fmt );
	
	// Determine the length of the resulting string, this method is much faster
	// than looping and reallocating a bigger buffer size.
	{	
		FILE* fnull = fopen( GWEN_FNULL, "wb" );
		va_list c;
		va_copy( c, s );
		len = vfwprintf( fnull, fmt, c );
		va_end( c );
		fclose( fnull );
	} 
	
	UnicodeString strOut;
	
	if (len > 0)
	{
		strOut.resize( len + 1 );
		va_list c;
		va_copy( c, s );
		len = vswprintf( &strOut[0], strOut.size(), fmt, c );
		va_end( c );
		strOut.resize( len );
	}
	
	va_end( s );
	
	return strOut;
}



void Gwen::Utility::Strings::Split( const Gwen::String & str, const Gwen::String & seperator, Strings::List & outbits, bool bLeave )
{
	int iOffset = 0;
	int iLength = str.length();
	int iSepLen = seperator.length();
	size_t i = str.find( seperator, 0 );

	while ( i != std::string::npos )
	{
		outbits.push_back( str.substr( iOffset, i - iOffset ) );
		iOffset = i + iSepLen;
		i = str.find( seperator, iOffset );

		if ( bLeave ) { iOffset -= iSepLen; }
	}

	outbits.push_back( str.substr( iOffset, iLength - iOffset ) );
}

void Gwen::Utility::Strings::Split( const Gwen::UnicodeString & str, const Gwen::UnicodeString & seperator, Strings::UnicodeList & outbits, bool bLeave )
{
	int iOffset = 0;
	int iLength = str.length();
	int iSepLen = seperator.length();
	size_t i = str.find( seperator, 0 );

	while ( i != std::wstring::npos )
	{
		outbits.push_back( str.substr( iOffset, i - iOffset ) );
		iOffset = i + iSepLen;
		i = str.find( seperator, iOffset );

		if ( bLeave ) { iOffset -= iSepLen; }
	}

	outbits.push_back( str.substr( iOffset, iLength - iOffset ) );
}

int Gwen::Utility::Strings::To::Int( const Gwen::String & str )
{
	if ( str == "" ) { return 0; }

	return atoi( str.c_str() );
}

float Gwen::Utility::Strings::To::Float( const Gwen::String & str )
{
	if ( str == "" ) { return 0.0f; }

	return ( float ) atof( str.c_str() );
}

float Gwen::Utility::Strings::To::Float( const Gwen::UnicodeString & str )
{
	return wcstod( str.c_str(), NULL );
}

bool Gwen::Utility::Strings::To::Bool( const Gwen::String & str )
{
	if ( str.size() == 0 ) { return false; }

	if ( str[0] == 'T' || str[0] == 't' || str[0] == 'y' || str[0] == 'Y' ) { return true; }

	if ( str[0] == 'F' || str[0] == 'f' || str[0] == 'n' || str[0] == 'N' ) { return false; }

	if ( str[0] == '0' ) { return false; }

	return true;
}

bool Gwen::Utility::Strings::To::Floats( const Gwen::String & str, float* f, size_t iCount )
{
	Strings::List lst;
	Strings::Split( str, " ", lst );

	if ( lst.size() != iCount ) { return false; }

	for ( size_t i = 0; i < iCount; i++ )
	{
		f[i] = Strings::To::Float( lst[i] );
	}

	return true;
}


bool Gwen::Utility::Strings::Wildcard( const TextObject & strWildcard, const TextObject & strHaystack )
{
	const UnicodeString & W = strWildcard.GetUnicode();
	const UnicodeString & H = strHaystack.GetUnicode();

	if ( strWildcard == "*" ) { return true; }

	int iPos = W.find( L"*", 0 );

	if ( iPos == UnicodeString::npos ) { return strWildcard == strHaystack; }

	// First half matches
	if ( iPos > 0 && W.substr( 0, iPos ) != H.substr( 0, iPos ) )
	{ return false; }

	// Second half matches
	if ( iPos != W.length() - 1 )
	{
		UnicodeString strEnd = W.substr( iPos + 1, W.length() );

		if ( strEnd != H.substr( H.length() - strEnd.length(), H.length() ) )
		{ return false; }
	}

	return true;
}



void Gwen::Utility::Strings::ToUpper( Gwen::UnicodeString & str )
{
	transform( str.begin(), str.end(), str.begin(), towupper );
}

void Gwen::Utility::Strings::Strip( Gwen::UnicodeString & str, const Gwen::UnicodeString & chars )
{
	Gwen::UnicodeString Source = str;
	str = L"";

	for ( unsigned int i = 0; i < Source.length(); i++ )
	{
		if ( chars.find( Source[i] ) != Gwen::UnicodeString::npos )
		{ continue; }

		str += Source[i];
	}
}

Color Gwen::Utility::HSVToColor( float h, float s, float v )
{
	if ( h < 0.0f ) { h += 360.0f; }

	if ( h > 360.0f ) { h -= 360.0f; }

	s *= 255.0f;
	v *= 255.0f;
	float r, g, b;

	if ( !h && !s )
	{
		r = g = b = v;
	}

	double min, max, delta, hue;
	max = v;
	delta = ( max * s ) / 255.0;
	min = max - delta;
	hue = h;

	if ( h > 300 || h <= 60 )
	{
		r = ( int ) max;

		if ( h > 300 )
		{
			g = ( int ) min;
			hue = ( hue - 360.0 ) / 60.0;
			b = ( int )( ( hue * delta - min ) * -1 );
		}
		else
		{
			b = ( int ) min;
			hue = hue / 60.0;
			g = ( int )( hue * delta + min );
		}
	}
	else if ( h > 60 && h < 180 )
	{
		g = ( int ) max;

		if ( h < 120 )
		{
			b = ( int ) min;
			hue = ( hue / 60.0 - 2.0 ) * delta;
			r = ( int )( min - hue );
		}
		else
		{
			r = ( int ) min;
			hue = ( hue / 60 - 2.0 ) * delta;
			b = ( int )( min + hue );
		}
	}
	else
	{
		b = ( int ) max;

		if ( h < 240 )
		{
			r = ( int ) min;
			hue = ( hue / 60.0 - 4.0 ) * delta;
			g = ( int )( min - hue );
		}
		else
		{
			g = ( int ) min;
			hue = ( hue / 60 - 4.0 ) * delta;
			r = ( int )( min + hue );
		}
	}

	return Color( r, g, b, 255 );
}

HSV Gwen::Utility::RGBtoHSV( int r, int g, int b )
{
	double min, max, delta, temp;
	min = Gwen::Min( r, Gwen::Min( g, b ) );
	max = Gwen::Max( r, Gwen::Max( g, b ) );
	delta = max - min;
	HSV hsv;
	hsv.v = ( int ) max;

	if ( !delta )
	{
		hsv.h = hsv.s = 0;
	}
	else
	{
		temp = delta / max;
		hsv.s = ( int )( temp * 255 );

		if ( r == ( int ) max )
		{
			temp = ( double )( g - b ) / delta;
		}
		else if ( g == ( int ) max )
		{
			temp = 2.0 + ( ( double )( b - r ) / delta );
		}
		else
		{
			temp = 4.0 + ( ( double )( r - g ) / delta );
		}

		temp *= 60;

		if ( temp < 0 )
		{
			temp += 360;
		}

		if ( temp == 360 )
		{
			temp = 0;
		}

		hsv.h = ( int ) temp;
	}

	hsv.s /= 255.0f;
	hsv.v /= 255.0f;
	return hsv;
}


