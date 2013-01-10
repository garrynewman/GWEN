
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class DocumentCanvas;

class Hierarchy : public Controls::Base 
{
	GWEN_CONTROL( Hierarchy, Controls::Base );

		void WatchCanvas( DocumentCanvas* pCanvas );

		void SelectNodeRepresentingControl( Controls::Base* pControl, Controls::TreeNode* pNode = NULL );

		void CompleteRefresh();

	protected:

		
		void UpdateNode( Controls::TreeNode* pNode, Controls::Base* pControls );

		void OnCanvasChildAdded( Event::Info info );
		void OnCanvasSelectionChanged( Event::Info info );

		void OnNodeSelected( Event::Info info );

		DocumentCanvas*			m_pCanvas;
		Controls::TreeControl*	m_Tree;

};