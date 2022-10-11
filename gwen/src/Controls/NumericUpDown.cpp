/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Utility.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/NumericUpDown.h"

using namespace Gwen;
using namespace Gwen::Controls;


GWEN_CONTROL_CONSTRUCTOR( NumericUpDown )
{
	SetSize( 100, 20 );
	Controls::Base* pSplitter = new Controls::Base( this );
	pSplitter->Dock( Pos::Right );
	pSplitter->SetWidth( 13 );
	NumericUpDownButton_Up* pButtonUp = new NumericUpDownButton_Up( pSplitter );
	pButtonUp->onPress.Add( this, &NumericUpDown::OnButtonUp );
	pButtonUp->SetTabable( false );
	pButtonUp->Dock( Pos::Top );
	pButtonUp->SetHeight( 10 );
	NumericUpDownButton_Down* pButtonDown = new NumericUpDownButton_Down( pSplitter );
	pButtonDown->onPress.Add( this, &NumericUpDown::OnButtonDown );
	pButtonDown->SetTabable( false );
	pButtonDown->Dock( Pos::Fill );
	pButtonUp->SetPadding( Padding( 0, 1, 1, 0 ) );
	m_iMax = 100;
	m_iMin = 0;
	m_iNumber = 0;
	m_iIncrement = 1;
	SetText( "0" );
}

void NumericUpDown::OnButtonUp( Base* /*control*/ )
{
	SyncNumberFromText();
	SetValue( m_iNumber + m_iIncrement, true );
}

void NumericUpDown::OnButtonDown( Base* /*control*/ )
{
	SyncNumberFromText();
	SetValue( m_iNumber - m_iIncrement, true );
}


void NumericUpDown::SyncTextFromNumber()
{
	SetText( Utility::ToString( m_iNumber ) );
}

void NumericUpDown::SyncNumberFromText()
{
	SetValue( ( int ) GetFloatFromText() );
}

void NumericUpDown::SetMin( int i )
{
	m_iMin = i;
}

void NumericUpDown::SetMax( int i )
{
	m_iMax = i;
}

void NumericUpDown::SetValue( int i, bool force )
{
	if ( i > m_iMax ) { i = m_iMax; }

	if ( i < m_iMin ) { i = m_iMin; }

	if ( m_iNumber == i )
	{
		return;
	}

	m_iNumber = i;
	// Don't update the text if we're typing in it..
	// Undone - any reason why not?
	if ( !HasFocus() || force)
	{
		SyncTextFromNumber();
	}
	OnChange();
}

void NumericUpDown::OnChange()
{
	onChanged.Call( this );
}

void NumericUpDown::OnTextChanged()
{
	BaseClass::OnTextChanged();
	SetValue( ( int ) GetFloatFromText(), false);
}

void NumericUpDown::OnEnter()
{
	SyncNumberFromText();
	SyncTextFromNumber();
	BaseClass::OnEnter();
}

void NumericUpDown::OnLostKeyboardFocus()
{
    BaseClass::OnLostKeyboardFocus();
    SyncNumberFromText();
	SyncTextFromNumber();
}

GWEN_CONTROL_CONSTRUCTOR( FloatUpDown )
{
	SetSize( 100, 20 );
	Controls::Base* pSplitter = new Controls::Base( this );
	pSplitter->Dock( Pos::Right );
	pSplitter->SetWidth( 13 );
	NumericUpDownButton_Up* pButtonUp = new NumericUpDownButton_Up( pSplitter );
	pButtonUp->onPress.Add( this, &FloatUpDown::OnButtonUp );
	pButtonUp->SetTabable( false );
	pButtonUp->Dock( Pos::Top );
	pButtonUp->SetHeight( 10 );
	NumericUpDownButton_Down* pButtonDown = new NumericUpDownButton_Down( pSplitter );
	pButtonDown->onPress.Add( this, &FloatUpDown::OnButtonDown );
	pButtonDown->SetTabable( false );
	pButtonDown->Dock( Pos::Fill );
	pButtonUp->SetPadding( Padding( 0, 1, 1, 0 ) );
	m_iMax = 100;
	m_iMin = 0;
	m_iNumber = 0;
	m_iIncrement = 1.0;
	SetText( "0" );
}

void FloatUpDown::OnButtonUp( Base* /*control*/ )
{
	SyncNumberFromText();
	SetValue( m_iNumber + m_iIncrement, true );
}

void FloatUpDown::OnButtonDown( Base* /*control*/ )
{
	SyncNumberFromText();
	SetValue( m_iNumber - m_iIncrement, true );
}


void FloatUpDown::SyncTextFromNumber()
{
	SetText( Utility::ToString( m_iNumber ) );
}

void FloatUpDown::SyncNumberFromText()
{
	SetValue( ( double ) GetFloatFromText() );
}

void FloatUpDown::SetMin( double i )
{
	m_iMin = i;
}

void FloatUpDown::SetMax( double i )
{
	m_iMax = i;
}

void FloatUpDown::SetValue( double i, bool force )
{
	if ( i > m_iMax ) { i = m_iMax; }

	if ( i < m_iMin ) { i = m_iMin; }

	if ( m_iNumber == i )
	{
		return;
	}

	m_iNumber = i;
	// Don't update the text if we're typing in it..
	// Undone - any reason why not?
	if ( !HasFocus() || force)
	{
		SyncTextFromNumber();
	}
	OnChange();
}

void FloatUpDown::OnChange()
{
	onChanged.Call( this );
}

void FloatUpDown::OnTextChanged()
{
	BaseClass::OnTextChanged();
	SetValue( ( double ) GetFloatFromText(), false );
}

void FloatUpDown::OnEnter()
{
	SyncNumberFromText();
	SyncTextFromNumber();
	BaseClass::OnEnter();
}

void FloatUpDown::OnLostKeyboardFocus()
{
    BaseClass::OnLostKeyboardFocus();
    SyncNumberFromText();
	SyncTextFromNumber();
}
