#include "Hierarchy.h"
#include "DocumentCanvas.h"
#include "Gwen/Util/ControlFactory.h"

GWEN_CONTROL_CONSTRUCTOR( Hierarchy )
{
	SetSize( 200, 200 );

	m_Tree = new Controls::TreeControl( this );
	m_Tree->Dock( Pos::Fill );
}

void Hierarchy::WatchCanvas( DocumentCanvas* pCanvas )
{
	m_pCanvas = pCanvas;
	m_pCanvas->onChildAdded.Add( this, &ThisClass::OnCanvasChildAdded );
	m_pCanvas->onSelectionChanged.Add( this, &ThisClass::OnCanvasSelectionChanged );

	CompleteRefresh();
}

void Hierarchy::CompleteRefresh()
{
	m_Tree->Clear();

	UpdateNode( m_Tree, m_pCanvas );

	m_Tree->ExpandAll();
}

void Hierarchy::UpdateNode( Controls::TreeNode* pNode, Controls::Base* pControl )
{	
	Controls::TreeNode* pChildNode = NULL;

	if ( !pControl->UserData.Exists( "ControlFactory" ) )
	{
		pChildNode = pNode;
	}
	else
	{
		Gwen::String strName = pControl->GetName();
		if ( strName == "" ) strName = "[" + Gwen::String( pControl->GetTypeName() ) + "]";

		pChildNode = pNode->AddNode( strName );
		pChildNode->SetImage( "img/controls/" + Gwen::String(pControl->GetTypeName()) + ".png" );
		pChildNode->onSelect.Add( this, &ThisClass::OnNodeSelected );
		pChildNode->UserData.Set<Controls::Base*>( "TargetControl", pControl );
	}


	for ( int i=0; i<pControl->NumChildren(); i++ )
	{
		UpdateNode( pChildNode, pControl->GetChild( i ) );
	}
}

void Hierarchy::OnCanvasChildAdded( Event::Info info )
{
	//info.Control;
	CompleteRefresh();
}

void Hierarchy::OnNodeSelected( Event::Info info )
{
	if ( !info.ControlCaller->UserData.Exists( "TargetControl" ) ) return;

	Controls::Base* ctrl = info.ControlCaller->UserData.Get<Controls::Base*>( "TargetControl" );

	ControlList list;
	list.Add( ctrl );

	m_pCanvas->SelectControls( list );
}

void Hierarchy::OnCanvasSelectionChanged( Event::Info info )
{
	m_Tree->DeselectAll();

	info.ControlList;

	for ( ControlList::List::const_iterator it = info.ControlList.list.begin(); it != info.ControlList.list.end(); ++it )
	{
		SelectNodeRepresentingControl( (*it), m_Tree );
	}
}

void Hierarchy::SelectNodeRepresentingControl( Controls::Base* pControl, Controls::TreeNode* pNode )
{
	if ( pNode == NULL ) pNode = m_Tree;

	if ( pNode->UserData.Exists( "TargetControl" ) && pNode->UserData.Get<Controls::Base*>( "TargetControl" ) == pControl )
	{
		pNode->SetSelected( true, false );
	}
	
	Base::List& children = pNode->GetChildNodes();
	for ( Base::List::iterator iter = children.begin(); iter != children.end(); ++iter )
	{
		Controls::TreeNode* pChildNode = gwen_cast<Controls::TreeNode>( *iter );
		if ( !pChildNode ) continue;

		SelectNodeRepresentingControl( pControl, pChildNode );
	}
}