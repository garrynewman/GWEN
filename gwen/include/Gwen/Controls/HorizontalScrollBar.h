/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_HORIZONTALSCROLLBAR_H
#define GWEN_CONTROLS_HORIZONTALSCROLLBAR_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/Dragger.h"
#include "Gwen/Controls/ScrollBar.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT HorizontalScrollBar : public BaseScrollBar
		{
			public:

				GWEN_CONTROL( HorizontalScrollBar, BaseScrollBar );

				virtual void Layout( Skin::Base* skin ) override;

				virtual void OnMouseClickLeft( int x, int y, bool bDown ) override;
				virtual void OnBarMoved( Controls::Base* control ) override;

				virtual int GetBarSize() override { return m_Bar->Width(); }
				virtual int GetBarPos() override { return m_Bar->X() - Height(); }
				virtual void SetBarSize( int size ) override { m_Bar->SetWidth( size ); }
				virtual int GetButtonSize() override { return Height(); }

				virtual void ScrollToLeft() override;
				virtual void ScrollToRight() override;
				void NudgeLeft( Base* control );
				void NudgeRight( Base* control );
				float GetNudgeAmount();

				virtual float CalculateScrolledAmount() override;
				virtual bool SetScrolledAmount( float amount, bool forceUpdate ) override;

				virtual bool IsHorizontal() override { return true; }
		};
	}
}
#endif
