/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_MENUSTRIP_H
#define GWEN_CONTROLS_MENUSTRIP_H

#include "Gwen/BaseRender.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Menu.h"
#include "Gwen/Controls/MenuItem.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT MenuStrip : public Menu
		{
				GWEN_CONTROL( MenuStrip, Menu );

				virtual void Render( Skin::Base* skin ) override;
				virtual void RenderUnder( Skin::Base* /*skin*/ ) override {}
				virtual void Layout( Skin::Base* skin ) override;

			protected:

				virtual void OnAddItem( MenuItem* item ) override;
				virtual bool ShouldHoverOpenMenu() override;
				virtual void Close() override {}
		};
	}

}
#endif
