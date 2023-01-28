/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_WINDOWCANVAS_H
#define GWEN_CONTROLS_WINDOWCANVAS_H

#include "Gwen/Controls/Base.h"
#include "Gwen/InputHandler.h"
#include "Gwen/WindowProvider.h"
#include "Gwen/Controls/Dragger.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/WindowButtons.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT WindowCanvas : public Canvas, public Gwen::WindowProvider
		{
			public:

				GWEN_CLASS( WindowCanvas, Controls::Canvas );

				WindowCanvas( int x, int y, int w, int h, Gwen::Skin::Base* pRenderer, const Gwen::String & strWindowTitle = "", bool is_menu = false );
				~WindowCanvas();

				virtual void DoThink();

				virtual bool WantsQuit() { return m_bQuit; }

				// Gwen::WindowProvider
				virtual void* GetWindow();

				virtual bool InputQuit();

				Skin::Base* GetSkin( void );

				virtual void Render( Skin::Base* skin );

				void SetWindowSize(int x, int y);
				virtual Gwen::Point WindowPosition() { return m_WindowPos; }
				
				virtual void OnMove(int x, int y) { m_WindowPos = Gwen::Point(x,y); }
				virtual void SetPos( int x, int y );
				virtual bool IsOnTop();

				virtual void Layout( Skin::Base* skin );

				virtual bool CanMaximize() { return m_bCanMaximize; }
				virtual void SetCanMaximize( bool b );
				virtual void SetMaximize( bool b );
				virtual void Minimize();

				virtual void OnChildRemoved( Controls::Base* pChild );

				virtual void SetSizable( bool b );
				virtual bool GetSizable() { return m_bHasTitleBar ? true : m_SESizer->Visible(); }
				virtual void SetMinimumSize( const Gwen::Point & minSize );
				virtual Gwen::Point GetMinimumSize() { return m_MinimumSize; }

				virtual void SetRemoveWhenChildless(bool yn) { m_bRemoveWhenChildless = yn; }

				virtual void SetTitle( Gwen::String title );

				virtual Gwen::Controls::Base* GetControlAt( int x, int y, bool bOnlyIfMouseEnabled );

				// DPI handling
				double GetDPI() { return m_dpi; }
				void SetDPI(const double d) { m_dpi = d; }
				Gwen::PointF GetDPIScaling() { auto dpi = GetDPI(); return Gwen::PointF(dpi / 96.0f, dpi / 96.0f); }

			protected:

				virtual void OnBoundsChanged( Gwen::Rect oldBounds );

				bool m_bRemoveWhenChildless = false;
				double m_dpi = 96.0;

				virtual void RenderCanvas();
				virtual void DestroyWindow();

				virtual void CloseButtonPressed();
				virtual void MaximizeButtonPressed();
				virtual void MinimizeButtonPressed();

				virtual void Dragger_Start();
				virtual void Dragger_Moved();
				virtual void SESizer_Moved();
				virtual void SWSizer_Moved();
				virtual void LeftSizer_Moved();
				virtual void RightSizer_Moved();
				virtual void VerticalSizer_Moved();
				virtual void OnTitleDoubleClicked();

				void*		m_pOSWindow;
				bool		m_bQuit;
				bool		m_bHasTitleBar;

				Gwen::Skin::Base*			m_pSkinChange;

				ControlsInternal::Dragger*	m_TitleBar;
				ControlsInternal::Dragger*	m_SWSizer;
				ControlsInternal::Dragger*	m_SESizer;
				ControlsInternal::Dragger*	m_RightSizer;
				ControlsInternal::Dragger*	m_LeftSizer;
				ControlsInternal::Dragger*	m_BottomSizer;
				Gwen::Controls::Label*		m_Title;


				Gwen::Point		m_WindowPos;
				Gwen::Point		m_HoldPos;
				Gwen::Point     m_WindowRightPos;
				
				Gwen::Point		m_MinimumSize;

				bool			m_bCanMaximize;
				bool			m_bIsMaximized;

				Gwen::Controls::WindowCloseButton*		m_pClose;
				Gwen::Controls::WindowMaximizeButton*	m_pMaximize;
				Gwen::Controls::WindowMinimizeButton*	m_pMinimize;

		};
	}
}
#endif
