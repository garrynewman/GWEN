/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Controls/ScrollControl.h"
#include "Gwen/Controls/ProgressBar.h"
#include "Gwen/Anim.h"
#include "Gwen/Utility.h"

using namespace Gwen;
using namespace Gwen::Controls;

class ProgressBarThink : public Gwen::Anim::Animation
{
	public:

		ProgressBarThink()
		{
			m_fLastFrame = 0.0f;
		}

		virtual void Think()
		{
			float fDiff = Platform::GetTimeInSeconds() - m_fLastFrame;
			gwen_cast<ProgressBar> ( m_Control )->CycleThink( Gwen::Clamp( fDiff, 0.f, 0.3f ) );
			m_fLastFrame = Platform::GetTimeInSeconds();
		}
		
		virtual bool Finished()
		{
			return false;
		}


		float	m_fLastFrame;
};


GWEN_CONTROL_CONSTRUCTOR( ProgressBar )
{
	SetMouseInputEnabled( true );
	SetBounds( Gwen::Rect( 0, 0, 128, 32 ) );
	SetTextPadding( Padding( 3, 3, 3, 3 ) );
	SetHorizontal();
	SetAlignment( Gwen::Pos::Center );
	m_fProgress = 0.0f;
	m_bAutoLabel = true;
	m_fCycleSpeed = 0.0f;
}

void ProgressBar::SetValue( float val )
{
	if ( val < 0 )
	{ val = 0; }

	if ( val > 1 )
	{ val = 1; }

	m_fProgress = val;

	if ( m_bAutoLabel )
	{
		int displayVal = m_fProgress * 100;
		SetText( Utility::ToString( displayVal ) + "%" );
	}
}

void ProgressBar::CycleThink( float fDelta )
{
	if ( !Visible() ) { return; }

	if ( m_fCycleSpeed == 0.0f ) { return; }

	m_fProgress += m_fCycleSpeed * fDelta;

	if ( m_fProgress < 0.0f ) { m_fProgress += 1.0f; }

	if ( m_fProgress > 1.0f ) { m_fProgress -= 1.0f; }

	Redraw();
}

void ProgressBar::Render( Skin::Base* skin )
{
	skin->DrawProgressBar( this, m_bHorizontal, m_fProgress );
}

float ProgressBar::GetCycleSpeed()
{
	return m_fCycleSpeed;
}

void ProgressBar::SetCycleSpeed( float f )
{
	if (f != m_fCycleSpeed)
	{
		if (f != 0.0f)
		{
			Gwen::Anim::Add( this, new ProgressBarThink() );
		}
		else
		{
			Gwen::Anim::Cancel( this );
		}
	}
	m_fCycleSpeed = f;
}
