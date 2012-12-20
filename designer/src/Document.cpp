#include "Document.h"
#include "DocumentCanvas.h"
#include "Hierarchy.h"
#include "Properties.h"

class DocumentInner : public Gwen::Controls::ScrollControl
{
	GWEN_CONTROL_INLINE( DocumentInner, Gwen::Controls::ScrollControl )
	{
	}

	void Render( Gwen::Skin::Base* skin )
	{
		skin->GetRender()->SetDrawColor( Color( 255, 255, 255, 255 ) );
		skin->GetRender()->DrawFilledRect( GetRenderBounds() );
	}

	void RenderOver( Gwen::Skin::Base* skin )
	{
		skin->GetRender()->SetDrawColor( Color( 90, 90, 90, 255 ) );
		skin->GetRender()->DrawLinedRect( GetRenderBounds() );
	}
};

GWEN_CONTROL_CONSTRUCTOR( Document )
{
	Dock( Pos::Fill );
	SetPadding( Padding( 1, 1, 1, 1 ) );

	// The main horizontal splitter separates the document from the tree/properties
	Controls::SplitterHorizontal* pSplitter = new Controls::SplitterHorizontal( this );
	pSplitter->Dock( Pos::Fill );
	pSplitter->SetScaling( true, 200 );

	// The white background
	DocumentInner* pInner = new DocumentInner( this );
	pInner->Dock( Pos::Fill );

	// The vertical splitter on the right containing the tree/properties
	Controls::SplitterVertical* pRightSplitter = new Controls::SplitterVertical( this );
	pRightSplitter->Dock( Pos::Fill );
	pRightSplitter->SetSize( 200, 200 );
	pRightSplitter->SetScaling( false, 200 );

	pSplitter->SetPanels( pInner, pRightSplitter );

	
	// The actual canvas onto which we drop controls
	{
		m_pCanvas = new DocumentCanvas( pInner );
		m_pCanvas->Dock( Pos::Fill );
		m_pCanvas->onHierachyChanged.Add( this, &ThisClass::OnHierachyChanged );
	}


	// The controls on the right
	{
		m_pHierarchy = new Hierarchy( pRightSplitter );
		m_pHierarchy->WatchCanvas( m_pCanvas );
		m_pHierarchy->Dock( Pos::Fill );

		Properties* pProperties = new Properties( pRightSplitter );
		pProperties->WatchCanvas( m_pCanvas );
		pProperties->Dock( Pos::Fill );

		pRightSplitter->SetPanels( m_pHierarchy, pProperties );
	}

}

void Document::Initialize( Controls::TabButton* pTab )
{

}

void Document::DoSaveFromDialog( Event::Info info )
{
	if ( info.String.Get() == "" ) return;
	if ( !m_Exporter ) return;

	m_strFilename = info.String.Get();
	
	m_Exporter->Export( m_pCanvas, m_strFilename );
	m_Exporter = NULL;
}

void Document::DoSave( ImportExport::Base* exporter )
{
	// We don't have a previous filename.. do save as
	if ( m_strFilename == "" )
	{
		return DoSaveAs( exporter );
	}

	exporter->Export( m_pCanvas, m_strFilename );
}

void Document::DoSaveAs( ImportExport::Base* exporter )
{
	m_Exporter = exporter;

	Gwen::Dialogs::FileSave( true, m_strFilename, "", "Gwen Designer File|*.gwen", this, &ThisClass::DoSaveFromDialog );
}

void Document::LoadFromFile( const Gwen::String& str, ImportExport::Base* exporter )
{
	exporter->Import( m_pCanvas, str );

	m_pHierarchy->CompleteRefresh();
}

void Document::OnHierachyChanged( Event::Info info )
{
	m_pHierarchy->CompleteRefresh();
}

void Document::Command( const Gwen::String& str )
{
	m_pCanvas->Command( str );

	m_pHierarchy->CompleteRefresh();
}