/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_CANVAS_H
#define GWEN_CONTROLS_CANVAS_H

#include <set>
#include "Gwen/Controls/Base.h"
#include "Gwen/InputHandler.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT Canvas : public Base
		{
			public:

				typedef Controls::Base BaseClass;

				Canvas( Skin::Base* pSkin );
				virtual ~Canvas();

				//
				// For additional initialization
				// (which is sometimes not appropriate in the constructor)
				//
				virtual void Initialize() {};

				//
				// You should call this to render your canvas.
				//
				virtual void RenderCanvas();

				//
				// Call this whenever you want to process input. This
				// is usually once a frame..
				//
				virtual void DoThink();

				//
				// In most situations you will be rendering the canvas
				// every frame. But in some situations you will only want
				// to render when there have been changes. You can do this
				// by checking NeedsRedraw().
				//
				virtual bool NeedsRedraw() { return m_bNeedsRedraw; }
				virtual void Redraw() { m_bNeedsRedraw = true; }

				// Internal. Do not call directly.
				virtual void Render( Skin::Base* pRender );

				// Childpanels call parent->GetCanvas() until they get to
				// this top level function.
				virtual Controls::Canvas* GetCanvas() { return this; }

				virtual void SetScale( float f );
				virtual float Scale() const { return m_fScale; }
				
				virtual void SetFontScale( float f);
				virtual float FontScale() const { return m_fFontScale; }

				virtual void OnBoundsChanged( Gwen::Rect oldBounds );

				// Delete all children (this is done called in the destructor too)
				virtual void ReleaseChildren();

				// Delayed deletes
				virtual void AddDelayedDelete( Controls::Base* pControl );
				virtual void ProcessDelayedDeletes();

				virtual bool SetSize(int w, int h);

				Controls::Base*	FirstTab;
				Controls::Base*	NextTab;

				// Input
				virtual bool InputQuit() { return true; };

				// Background
				virtual void SetBackgroundColor( const Gwen::Color & color ) { m_BackgroundColor = color; }
				virtual void SetDrawBackground( bool bShouldDraw ) { m_bDrawBackground = bShouldDraw; }
				
				virtual Gwen::Point WindowPosition() { return Gwen::Point(0, 0); }

			protected:
				
				int real_size_x;
				int real_size_y;

				bool	m_bNeedsRedraw;
				bool	m_bAnyDelete;
				float	m_fScale;
				float   m_fFontScale;

				Controls::Base::List	m_DeleteList;
				std::set< Controls::Base* > m_DeleteSet;
				friend class Controls::Base;
				void PreDeleteCanvas( Controls::Base* );

				bool			m_bDrawBackground;
				Gwen::Color		m_BackgroundColor;


		};
	}
}
#endif
