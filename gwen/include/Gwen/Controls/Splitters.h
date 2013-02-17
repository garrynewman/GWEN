/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_SPLITTERS_H
#define GWEN_CONTROLS_SPLITTERS_H

#include "Gwen/Controls/Base.h"

namespace Gwen
{
	namespace Controls
	{
		class GWEN_EXPORT SplitterVertical : public Controls::Base
		{
				GWEN_CONTROL_INLINE( SplitterVertical, Controls::Base )
				{
					m_Panels[0] = new Controls::Base( this );
					m_Panels[1] = new Controls::Base( this );
					m_pSplitter = new Controls::SplitterBar( this );
					m_SplitterSize = 6;
					m_pSplitter->SetPos( 0, 100 );
					m_pSplitter->SetCursor( Gwen::CursorType::SizeNS );
					m_pSplitter->onDragged.Add( this, &ThisClass::OnSplitterMoved );
					SetScaling( false, 100 );
				}

				virtual void PostLayout( Skin::Base* skin )
				{
					RefreshContainers();
				}

				void SetPanels( Controls::Base* pA, Controls::Base* pB )
				{
					if ( pA ) { pA->SetParent( m_Panels[0] ); }

					if ( pB ) { pB->SetParent( m_Panels[1] ); }
				}

				void SetScaling( bool Right, int iSize )
				{
					m_RightSided = Right;
					m_Size = iSize;
				}

				virtual int SplitterPos()
				{
					return m_pSplitter->Y();
				}

			protected:

				virtual void RefreshContainers()
				{
					const Gwen::Rect & inner = GetInnerBounds();
					int iOffset = m_Size;

					if ( m_RightSided ) { iOffset = Height() - m_Size; }

					m_pSplitter->SetSize( inner.w, m_SplitterSize );
					m_pSplitter->SetPos( 0, iOffset );
					m_Panels[0]->SetPos( inner.x, inner.y );
					m_Panels[0]->SetSize( inner.w, iOffset );
					m_Panels[1]->SetPos( inner.x, ( iOffset + m_SplitterSize ) );
					m_Panels[1]->SetSize( inner.w, ( inner.y + inner.h ) - ( iOffset + m_SplitterSize ) );
				}

				virtual void OnSplitterMoved()
				{
					if ( m_RightSided )
					{ m_Size = Height() - m_pSplitter->Y(); }
					else
					{ m_Size = m_pSplitter->Y(); }

					RefreshContainers();
					Invalidate();
				}

				Controls::Base*			m_Panels[2];
				bool					m_RightSided;
				int						m_Size;
				Controls::SplitterBar*	m_pSplitter;
				unsigned int			m_SplitterSize;
		};

		class GWEN_EXPORT SplitterHorizontal : public SplitterVertical
		{
				GWEN_CONTROL_INLINE( SplitterHorizontal, SplitterVertical )
				{
					m_pSplitter->SetCursor( Gwen::CursorType::SizeWE );
					m_pSplitter->SetPos( 100, 0 );
				}

				virtual void RefreshContainers()
				{
					const Gwen::Rect & inner = GetInnerBounds();
					int iOffset = m_Size;

					if ( m_RightSided ) { iOffset = Width() - m_Size; }

					m_pSplitter->SetSize( m_SplitterSize, inner.h );
					m_pSplitter->SetPos( iOffset, 0 );
					m_Panels[0]->SetPos( inner.x, inner.y );
					m_Panels[0]->SetSize( iOffset, inner.h );
					m_Panels[1]->SetPos( ( iOffset + m_SplitterSize ), inner.y );
					m_Panels[1]->SetSize( ( inner.x + inner.w ) - ( iOffset + m_SplitterSize ), inner.h );
				}

				virtual void OnSplitterMoved()
				{
					if ( m_RightSided )
					{ m_Size = Width() - m_pSplitter->X(); }
					else
					{ m_Size = m_pSplitter->X(); }

					RefreshContainers();
					Invalidate();
				}

				virtual int SplitterPos()
				{
					return m_pSplitter->X();
				}
		};
	}
}

#endif
