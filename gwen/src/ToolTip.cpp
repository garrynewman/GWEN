/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/ToolTip.h"
#include "Gwen/Utility.h"
#include "Gwen/Platform.h"
#include "Gwen/Anim.h"

using namespace Gwen;
using namespace Gwen::Controls;

namespace ToolTip
{
	Base* g_ToolTip = NULL;
	
#ifndef GWEN_NO_ANIMATION
	bool g_ShowToolTip = false;
	class TooltipDelay : public Anim::TimedAnimation
	{
	public:
	
		TooltipDelay( float fDelay = 0.0f ) : Anim::TimedAnimation( 0.0f, fDelay ) {}
		virtual void OnFinish() { m_Control->Redraw(); g_ShowToolTip = true; }
	};
#endif

	GWEN_EXPORT bool TooltipActive()
	{
		return g_ToolTip != NULL;
	}

	void Enable( Controls::Base* pControl )
	{
		if ( !pControl->GetToolTip() )
		{ return; }

#ifndef GWEN_NO_ANIMATION
		Gwen::Anim::Cancel(g_ToolTip);
		Gwen::Anim::Add(pControl, new TooltipDelay(0.8));// change me to adjust tooltip delay
#endif
		g_ToolTip = pControl;
	}

	void Disable( Controls::Base* pControl )
	{
		if ( g_ToolTip == pControl )
		{
			g_ToolTip = NULL;
			g_ShowToolTip = false;
			Gwen::Anim::Cancel(pControl);
		}
	}

	void Reset( )
	{
#ifndef GWEN_NO_ANIMATION
		if (g_ToolTip)
		{
			if (g_ShowToolTip)
			{
				g_ToolTip->Redraw();
			}
			g_ShowToolTip = false;
			Gwen::Anim::Cancel(g_ToolTip);
			Gwen::Anim::Add(g_ToolTip, new TooltipDelay(0.8));// change me to adjust tooltip delay
		}
#endif
	}

	void RenderToolTip( Controls::Canvas* canvas, Skin::Base* skin )
	{
		if ( !g_ToolTip ) { return; }
		
		if (canvas != g_ToolTip->GetCanvas()) { return; }
		
#ifndef GWEN_NO_ANIMATION
		if (!g_ShowToolTip) { return; }
#endif

		Gwen::Renderer::Base* render = skin->GetRender();
		Gwen::Point pOldRenderOffset = render->GetRenderOffset();
		Gwen::Point MousePos = Input::GetMousePosition();
		MousePos -= canvas->WindowPosition();
		MousePos.x /= canvas->Scale();
		MousePos.y /= canvas->Scale();
		Gwen::Rect Bounds = g_ToolTip->GetToolTip()->GetBounds();
		Gwen::Rect rOffset = Gwen::Rect( MousePos.x - Bounds.w * 0.5f, MousePos.y - Bounds.h - 10, Bounds.w, Bounds.h );
		rOffset = Utility::ClampRectToRect( rOffset, g_ToolTip->GetCanvas()->GetBounds() );
		//Calculate offset on screen bounds
		render->AddRenderOffset( rOffset );
		render->EndClip();
		skin->DrawToolTip( g_ToolTip->GetToolTip() );
		g_ToolTip->GetToolTip()->DoRender( skin );
		render->SetRenderOffset( pOldRenderOffset );
	}

	void ControlDeleted( Controls::Base* pControl )
	{
		Disable( pControl );
	}
}
