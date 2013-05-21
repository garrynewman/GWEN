/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include "Gwen/Utility.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/PageControl.h"
#include "Gwen/Controls.h"

using namespace Gwen;
using namespace Gwen::Controls;


GWEN_CONTROL_CONSTRUCTOR( PageControl )
{
	m_iPages = 0;
	m_iCurrentPage = 0;
	SetUseFinishButton( true );

	for ( int i = 0; i < MaxPages; i++ )
	{
		m_pPages[i] = NULL;
	}

	Base* pControls = new Base( this );
	pControls->Dock( Pos::Bottom );
	pControls->SetSize( 24, 24 );
	pControls->SetMargin( Margin( 10, 10, 10, 10 ) );
	m_Finish = new Controls::Button( pControls );
	m_Finish->SetText( "Finish" );
	m_Finish->Dock( Pos::Right );
	m_Finish->onPress.Add( this, &ThisClass::Finish );
	m_Finish->SetSize( 70 );
	m_Finish->SetMargin( Margin( 4, 0, 0, 0 ) );
	m_Finish->Hide();
	m_Next = new Controls::Button( pControls );
	m_Next->SetText( "Next >" );
	m_Next->Dock( Pos::Right );
	m_Next->onPress.Add( this, &ThisClass::NextPage );
	m_Next->SetSize( 70 );
	m_Next->SetMargin( Margin( 4, 0, 0, 0 ) );
	m_Back = new Controls::Button( pControls );
	m_Back->SetText( "< Back" );
	m_Back->Dock( Pos::Right );
	m_Back->onPress.Add( this, &ThisClass::PreviousPage );
	m_Back->SetSize( 70 );
	m_Label = new Controls::Label( pControls );
	m_Label->Dock( Pos::Fill );
	m_Label->SetAlignment( Pos::Left | Pos::CenterV );
	m_Label->SetText( "Page 1 or 2" );
}

void PageControl::SetPageCount( unsigned int iNum )
{
	if ( iNum >= MaxPages ) { iNum = MaxPages; }

	for ( unsigned int i = 0; i < iNum; i++ )
	{
		if ( !m_pPages[i] )
		{
			m_pPages[i] = new Controls::Base( this );
			m_pPages[i]->Dock( Pos::Fill );
		}
	}

	m_iPages = iNum;
	// Setting to -1 to force the page to change
	m_iCurrentPage = -1;
	HideAll();
	ShowPage( 0 );
}

void PageControl::HideAll()
{
	for ( int i = 0; i < MaxPages; i++ )
	{
		if ( !m_pPages[i] ) { continue; }

		m_pPages[i]->Hide();
	}
}

void PageControl::ShowPage( unsigned int i )
{
	if ( m_iCurrentPage == i ) { return; }

	if ( m_pPages[i] )
	{
		m_pPages[i]->Show();
		m_pPages[i]->Dock( Pos::Fill );
	}

	m_iCurrentPage = i;
	m_Back->SetDisabled( m_iCurrentPage == 0 );
	m_Next->SetDisabled( m_iCurrentPage >= m_iPages );
	m_Label->SetText( Utility::Format( L"Page %i of %i", m_iCurrentPage + 1, m_iPages ) );

	if ( GetUseFinishButton() )
	{
		bool bFinished = m_iCurrentPage >= m_iPages - 1;
		m_Next->SetHidden( bFinished );
		m_Finish->SetHidden( !bFinished );
	}

	{
		Event::Information info;
		info.Integer = i;
		info.Control = m_pPages[i];
		onPageChanged.Call( this, info );
	}
}

Controls::Base* PageControl::GetPage( unsigned int i )
{
	return m_pPages[i];
}

Controls::Base* PageControl::GetCurrentPage()
{
	return GetPage( GetPageNumber() );
}

void PageControl::NextPage()
{
	if ( m_iCurrentPage >= m_iPages - 1 ) { return; }

	if ( m_pPages[m_iCurrentPage] )
	{
		m_pPages[m_iCurrentPage]->Dock( Pos::None );
		Anim::Add( m_pPages[m_iCurrentPage], new Anim::Pos::X( m_pPages[m_iCurrentPage]->X(), Width() * -1, 0.2f, true, 0.0f, -1 ) );
	}

	ShowPage( m_iCurrentPage + 1 );

	if ( m_pPages[m_iCurrentPage] )
	{
		m_pPages[m_iCurrentPage]->Dock( Pos::None );
		Anim::Add( m_pPages[m_iCurrentPage], new Anim::Pos::X( Width(), 0, 0.2f, false, 0.0f, -1 ) );
	}
}

void PageControl::PreviousPage()
{
	if ( m_iCurrentPage == 0 ) { return; }

	if ( m_pPages[m_iCurrentPage] )
	{
		m_pPages[m_iCurrentPage]->Dock( Pos::None );
		Anim::Add( m_pPages[m_iCurrentPage], new Anim::Pos::X( m_pPages[m_iCurrentPage]->X(), Width(), 0.3f, true, 0.0f, -1 ) );
	}

	ShowPage( m_iCurrentPage - 1 );

	if ( m_pPages[m_iCurrentPage] )
	{
		m_pPages[m_iCurrentPage]->Dock( Pos::None );
		Anim::Add( m_pPages[m_iCurrentPage], new Anim::Pos::X( Width() * -1, 0, 0.3f, false, 0.0f, -1 ) );
	}
}

void PageControl::Finish()
{
	onFinish.Call( this );
}