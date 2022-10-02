/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_TOOLTIP_H
#define GWEN_CONTROLS_TOOLTIP_H

#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"
#include "Gwen/Controls/Label.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT BasicToolTip : public Label
		{
			public:

				GWEN_CONTROL_INLINE( BasicToolTip, Controls::Label )
				{

                }

                virtual void OnScaleChanged()
                {
                    Base::OnScaleChanged();

                    SizeToContents();
                    Invalidate();
                }

			private:

		};
	}
}
#endif
