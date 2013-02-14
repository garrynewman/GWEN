/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_ALIGN_H
#define GWEN_ALIGN_H
#include "Gwen/Controls/Base.h"

namespace Gwen
{
	namespace Align
	{
		inline void PlaceBelow( Controls::Base* ctrl, Controls::Base* below, int iBorder = 0 )
		{
			ctrl->SetPos( ctrl->X(), below->Bottom() + iBorder );
		}
	}
}
#endif
