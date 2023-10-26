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
				virtual ~WindowCanvas();

				virtual void DoThink() override;


				// Gwen::WindowProvider
				virtual void* GetWindow() override;

				virtual bool InputQuit() override;
				bool WantsQuit() { return m_bQuit; }

				Skin::Base* GetSkin( void );

				virtual void Render( Skin::Base* skin );

				void SetWindowSize(int x, int y);
				void SetWindowPosition(Gwen::Point pos);
				Gwen::Point WindowPosition() { return m_WindowPos; }
				
				virtual void OnMove(int x, int y) { m_WindowPos = Gwen::Point(x,y); }
				virtual bool IsOnTop() override;

				virtual void Layout( Skin::Base* skin ) override;

				bool CanMaximize() { return m_bCanMaximize; }
				void SetCanMaximize( bool b );
				void SetMaximize( bool b );
				void Minimize();

				virtual void OnChildRemoved( Controls::Base* pChild ) override;

				void SetSizable( bool b );
				bool GetSizable() { return m_bHasTitleBar ? true : m_SESizer->Visible(); }
				void SetMinimumSize( const Gwen::Point & minSize );
				virtual Gwen::Point GetMinimumSize() override { return m_MinimumSize; }

				void SetRemoveWhenChildless(bool yn) { m_bRemoveWhenChildless = yn; }

				void SetTitle( Gwen::String title );

				void SetPos(int x, int y);

				virtual Gwen::Controls::Base* GetControlAt( int x, int y, bool bOnlyIfMouseEnabled ) override;

				// DPI handling
				double GetDPI() { return m_dpi; }
				void SetDPI(const double d) { m_dpi = d; }
				Gwen::PointF GetDPIScaling() { auto dpi = GetDPI(); return Gwen::PointF(dpi / 96.0f, dpi / 96.0f); }

			protected:

				bool m_bRemoveWhenChildless = false;
				double m_dpi = 96.0;

				virtual void RenderCanvas() override;
				void DestroyWindow();

				void CloseButtonPressed();
				void MaximizeButtonPressed();
				void MinimizeButtonPressed();

				void Dragger_Start();
				void Dragger_Moved();
				void SESizer_Moved();
				void SWSizer_Moved();
				void LeftSizer_Moved();
				void RightSizer_Moved();
				void VerticalSizer_Moved();
				void OnTitleDoubleClicked();

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
