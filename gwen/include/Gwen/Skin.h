/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_SKIN_H
#define GWEN_SKIN_H

#include "Gwen/BaseRender.h"
#include "Gwen/Font.h"

namespace Gwen
{
	namespace Controls
	{
		class Base;
	}

	namespace Skin
	{
		namespace Symbol
		{
			static const unsigned char None				= 0;
			static const unsigned char ArrowRight		= 1;
			static const unsigned char Check			= 2;
			static const unsigned char Dot				= 3;
		}

		class GWEN_EXPORT Base
		{
			public:

				Base( Gwen::Renderer::Base* renderer = NULL )
				{
					m_DefaultFont.facename = L"Arial";
					m_DefaultFont.size = 10.0f;
					m_Render = renderer;
				}

				virtual ~Base()
				{
					ReleaseFont( &m_DefaultFont );
				}

				virtual void ReleaseFont( Gwen::Font* fnt )
				{
					if ( !fnt ) { return; }

					if ( !m_Render ) { return; }

					m_Render->FreeFont( fnt );
				}

				virtual void DrawGenericPanel( Controls::Base* control ) = 0;

				virtual void DrawButton( Controls::Base* control, bool bDepressed, bool bHovered, bool bDisabled ) = 0;
				virtual void DrawTabButton( Controls::Base* control, bool bActive, int dir, bool bHovered ) = 0;
				virtual void DrawTabControl( Controls::Base* control ) = 0;
				virtual void DrawTabTitleBar( Controls::Base* control ) = 0;


				virtual void DrawMenuItem( Controls::Base* control, bool bSubmenuOpen, bool bChecked, bool bHovered ) = 0;
				virtual void DrawMenuStrip( Controls::Base* control ) = 0;
				virtual void DrawMenu( Controls::Base* control, bool bPaddingDisabled ) = 0;
				virtual void DrawMenuRightArrow( Controls::Base* control ) = 0;

				virtual void DrawRadioButton( Controls::Base* control, bool bSelected, bool bDepressed, bool bHovered ) = 0;
				virtual void DrawCheckBox( Controls::Base* control, bool bSelected, bool bDepressed, bool bHovered ) = 0;
				virtual void DrawGroupBox( Controls::Base* control, int textStart, int textHeight, int textWidth ) = 0;
				virtual void DrawTextBox( Controls::Base* control ) = 0;

				virtual void DrawWindow( Controls::Base* control, int topHeight, bool inFocus ) = 0;
				virtual void DrawWindowCloseButton( Gwen::Controls::Base* control, bool bDepressed, bool bHovered, bool bDisabled ) = 0;
				virtual void DrawWindowMaximizeButton( Gwen::Controls::Base* control, bool bDepressed, bool bHovered, bool bDisabled, bool bMaximized ) = 0;
				virtual void DrawWindowMinimizeButton( Gwen::Controls::Base* control, bool bDepressed, bool bHovered, bool bDisabled ) = 0;


				virtual void DrawHighlight( Controls::Base* control ) = 0;
				virtual void DrawStatusBar( Controls::Base* control ) = 0;

				virtual void DrawShadow( Controls::Base* control ) = 0;
				virtual void DrawScrollBarBar( Controls::Base* control, bool bDepressed, bool isHovered, bool isHorizontal ) = 0;
				virtual void DrawScrollBar( Controls::Base* control, bool isHorizontal, bool bDepressed ) = 0;
				virtual void DrawScrollButton( Controls::Base* control, int iDirection, bool bDepressed, bool bHovered, bool bDisabled ) = 0;
				virtual void DrawProgressBar( Controls::Base* control, bool isHorizontal, float progress ) = 0;

				virtual void DrawListBox( Controls::Base* control ) = 0;
				virtual void DrawListBoxLine( Controls::Base* control, bool bSelected, bool bEven, bool bHovered ) = 0;

				virtual void DrawSlider( Controls::Base* control, bool bIsHorizontal, int numNotches, int barSize ) = 0;
				virtual void DrawSlideButton( Gwen::Controls::Base* control, bool bDepressed, bool bHorizontal, bool bHovered ) = 0;

				virtual void DrawComboBox( Controls::Base* control, bool bIsDown, bool bIsMenuOpen, bool bHovered ) = 0;
				virtual void DrawComboDownArrow( Gwen::Controls::Base* control, bool bHovered, bool bDown, bool bOpen, bool bDisabled ) = 0;
				virtual void DrawKeyboardHighlight( Controls::Base* control, const Gwen::Rect & rect, int offset ) = 0;
				virtual void DrawToolTip( Controls::Base* control ) = 0;

				virtual void DrawNumericUpDownButton( Controls::Base* control, bool bDepressed, bool bUp, bool bHovered ) = 0;

				virtual void DrawTreeButton( Controls::Base* control, bool bOpen ) = 0;
				virtual void DrawTreeControl( Controls::Base* control ) = 0;
				virtual void DrawTreeNode( Controls::Base* ctrl, bool bOpen, bool bSelected, int iLabelHeight, int iLabelWidth, int iHalfWay, int iLastBranch, bool bIsRoot );

				virtual void DrawPropertyRow( Controls::Base* control, int iWidth, bool bBeingEdited, bool bHovered );
				virtual void DrawPropertyTreeNode( Controls::Base* control, int BorderLeft, int BorderTop );
				virtual void DrawColorDisplay( Controls::Base* control, Gwen::Color color ) = 0;
				virtual void DrawModalControl( Controls::Base* control ) = 0;
				virtual void DrawMenuDivider( Controls::Base* control ) = 0;

				virtual void DrawCategoryHolder( Controls::Base* ctrl ) = 0;
				virtual void DrawCategoryInner( Controls::Base* ctrl, int header_height, bool bCollapsed ) = 0;


				virtual void SetRender( Gwen::Renderer::Base* renderer )
				{
					m_Render = renderer;
				}
				virtual Gwen::Renderer::Base* GetRender()
				{
					return m_Render;
				}

				virtual void DrawArrowDown( Gwen::Rect rect );
				virtual void DrawArrowUp( Gwen::Rect rect );
				virtual void DrawArrowLeft( Gwen::Rect rect );
				virtual void DrawArrowRight( Gwen::Rect rect );
				virtual void DrawCheck( Gwen::Rect rect );


				struct
				{
					struct Window_t
					{
						Gwen::Color TitleActive;
						Gwen::Color TitleInactive;

					} Window;

					struct Label_t
					{
						Gwen::Color Default;
						Gwen::Color Bright;
						Gwen::Color Dark;
						Gwen::Color Highlight;

					} Label;

					struct Tree_t
					{
						Gwen::Color Lines;
						Gwen::Color Normal;
						Gwen::Color Hover;
						Gwen::Color Selected;

					} Tree;

					struct Properties_t
					{
						Gwen::Color Line_Normal;
						Gwen::Color Line_Selected;
						Gwen::Color Line_Hover;
						Gwen::Color Column_Normal;
						Gwen::Color Column_Selected;
						Gwen::Color Column_Hover;
						Gwen::Color Label_Normal;
						Gwen::Color Label_Selected;
						Gwen::Color Label_Hover;
						Gwen::Color Border;
						Gwen::Color Title;

					} Properties;

					struct Button_t
					{
						Gwen::Color Normal;
						Gwen::Color Hover;
						Gwen::Color Down;
						Gwen::Color Disabled;

					} Button;

					struct Tab_t
					{
						struct Active_t
						{
							Gwen::Color Normal;
							Gwen::Color Hover;
							Gwen::Color Down;
							Gwen::Color Disabled;
						} Active;

						struct Inactive_t
						{
							Gwen::Color Normal;
							Gwen::Color Hover;
							Gwen::Color Down;
							Gwen::Color Disabled;
						} Inactive;

					} Tab;

					struct Category_t
					{
						Gwen::Color Header;
						Gwen::Color Header_Closed;

						struct Line_t
						{
							Gwen::Color Text;
							Gwen::Color Text_Hover;
							Gwen::Color Text_Selected;
							Gwen::Color Button;
							Gwen::Color Button_Hover;
							Gwen::Color Button_Selected;
						} Line;

						struct LineAlt_t
						{
							Gwen::Color Text;
							Gwen::Color Text_Hover;
							Gwen::Color Text_Selected;
							Gwen::Color Button;
							Gwen::Color Button_Hover;
							Gwen::Color Button_Selected;
						} LineAlt;

					} Category;

					Gwen::Color ModalBackground;
					Gwen::Color TooltipText;

				} Colors;


			public:

				virtual Gwen::Font* GetDefaultFont()
				{
					return &m_DefaultFont;
				}

				virtual void SetDefaultFont( const Gwen::UnicodeString & strFacename, float fSize = 10.0f )
				{
					m_DefaultFont.facename = strFacename;
					m_DefaultFont.size = fSize;
				}

			protected:

				Gwen::Font m_DefaultFont;
				Gwen::Renderer::Base* m_Render;

		};
	};
}
#endif
