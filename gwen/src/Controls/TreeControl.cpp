/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Controls/TreeControl.h"
#include "Gwen/Controls/ScrollControl.h"
#include "Gwen/Utility.h"

using namespace Gwen;
using namespace Gwen::Controls;

GWEN_CONTROL_CONSTRUCTOR( TreeControl )
{
	m_TreeControl = this;
	m_ToggleButton->DelayedDelete();
	m_ToggleButton = NULL;
	m_Title->DelayedDelete();
	m_Title = NULL;
	m_InnerPanel->DelayedDelete();
	m_InnerPanel = NULL;
	m_bAllowMultipleSelection = false;
	m_ScrollControl = new ScrollControl( this );
	m_ScrollControl->Dock( Pos::Fill );
	m_ScrollControl->SetScroll( false, true );
	m_ScrollControl->SetAutoHideBars( true );
	m_ScrollControl->SetMargin( Margin( 1, 1, 1, 1 ) );
	m_InnerPanel = m_ScrollControl;
	m_ScrollControl->SetInnerSize( 1000, 1000 );
}

void TreeControl::Render( Skin::Base* skin )
{
	if ( ShouldDrawBackground() )
	{ skin->DrawTreeControl( this ); }
}

void TreeControl::OnChildBoundsChanged( Gwen::Rect /*oldChildBounds*/, Base* /*pChild*/ )
{
	m_ScrollControl->UpdateScrollBars();
	Invalidate();
}

void TreeControl::Clear()
{
	m_ScrollControl->Clear();
}

void TreeControl::Layout( Skin::Base* skin )
{
	BaseClass::BaseClass::Layout( skin );
}

void TreeControl::PostLayout( Skin::Base* skin )
{
	BaseClass::BaseClass::PostLayout( skin );
}

void TreeControl::OnNodeAdded( TreeNode* pNode )
{
	pNode->onNamePress.Add( this, &TreeControl::OnNodeSelection );
}

void TreeControl::OnNodeSelection( Controls::Base* control )
{
	if (!m_bAllowMultipleSelection)
	{
		DeselectAll();
		return;
	}

	if (Gwen::Input::IsKeyDown( Key::Control ))
	{
		return;
	}
	else if (Gwen::Input::IsKeyDown( Key::Shift ))
	{
		auto& children = m_InnerPanel->GetChildren();
		int min_selected = children.size() + 1;
		int max_selected = -1;
		int selected_index = 0;

		int index = 0;
		for ( Base::List::iterator iter = children.begin(); iter != children.end(); ++iter )
		{
			TreeNode* pChild = gwen_cast<TreeNode> ( *iter );

			if (*iter == control)
			{
				selected_index = index;
			}

			if ( pChild->IsSelected() )
			{
				min_selected = std::min(min_selected, index);
				max_selected = std::max(max_selected, index);
			}

			index++;
		}
        
		index = 0;
		if (max_selected == -1)
		{
			// nothing else was selected so just select the current one
		}
		else if (selected_index < min_selected)
		{
			for ( Base::List::iterator iter = children.begin(); iter != children.end(); ++iter )
			{
				TreeNode* pChild = gwen_cast<TreeNode> ( *iter );
				if (index < min_selected && index > selected_index)
				{
					pChild->SetSelected(true);
				}
				index++;
			}
		}
		else if (selected_index > max_selected)
		{
			for ( Base::List::iterator iter = children.begin(); iter != children.end(); ++iter )
			{
				TreeNode* pChild = gwen_cast<TreeNode> ( *iter );
				if (index > min_selected && index < selected_index)
				{
					pChild->SetSelected(true);
				}
				index++;
			}
		}
		return;
	}

	DeselectAll();
}
