/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_VERTICALSCROLLBAR_H
#define GWEN_CONTROLS_VERTICALSCROLLBAR_H
#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/ScrollBar.h"
#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"

namespace Gwen
{
	namespace Controls
	{

		class GWEN_EXPORT VerticalScrollBar : public BaseScrollBar
		{

				GWEN_CONTROL( VerticalScrollBar, BaseScrollBar );

				virtual void Layout( Skin::Base* skin ) override;

				virtual void OnMouseClickLeft( int x, int y, bool bDown ) override;
				virtual void OnBarMoved( Controls::Base* control ) override;

				virtual int GetBarSize() override { return  m_Bar->Height(); }
				virtual int GetBarPos() override { return m_Bar->Y() - Width(); }
				virtual void SetBarSize( int size ) override { m_Bar->SetHeight( size ); }
				virtual int GetButtonSize() override { return Width(); }

				virtual void ScrollToTop() override;
				virtual void ScrollToBottom() override;
				void NudgeUp( Base* control );
				void NudgeDown( Base* control );
				virtual float GetNudgeAmount() override;

				virtual float CalculateScrolledAmount() override;
				virtual bool SetScrolledAmount( float amount, bool forceUpdate ) override;

		};
	}
}
#endif
