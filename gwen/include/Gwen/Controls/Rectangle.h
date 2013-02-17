/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_RECTANGLE_H
#define GWEN_CONTROLS_RECTANGLE_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"


namespace Gwen
{
	namespace Controls
	{

		class GWEN_EXPORT Rectangle : public Controls::Base
		{
			public:

				GWEN_CONTROL( Rectangle, Controls::Base );

				virtual void Render( Skin::Base* skin );

				const Gwen::Color & GetColor() { return m_Color; }
				void SetColor( const Gwen::Color & col ) { m_Color = col; }

			protected:

				Gwen::Color		m_Color;

		};
	}
}
#endif
