#include "DesignerBase.h"
#include "Document.h"
#include "ControlToolbox.h"
#include "Gwen/Util/ImportExport.h"

GWEN_CONTROL_CONSTRUCTOR( DesignerBase )
{
	Dock( Pos::Fill );

	CreateMenu();
	CreateToolBar();
	CreateControlToolbox();
	CreateDocumentHolder();

	new Controls::StatusBar( this, "StatusBar" );

	NewDocument();
}


void DesignerBase::CreateMenu()
{
	Controls::MenuStrip* pStrip = new Controls::MenuStrip( this );
	pStrip->Dock( Pos::Top );

	// File
	{
		Gwen::Controls::MenuItem* pRoot = pStrip->AddItem( "File" );

		pRoot->GetMenu()->AddItem( "New", "img/menu/new.png", "Ctrl + N" )->SetAction( this, &ThisClass::NewDocument );

		pRoot->GetMenu()->AddItem( "Open", "img/menu/open.png", "Ctrl + O" )->SetAction( this, &ThisClass::OpenDocument );
		pRoot->GetMenu()->AddItem( "Save", "img/menu/save.png", "Ctrl + S" )->SetAction( this, &ThisClass::SaveDocument );
		pRoot->GetMenu()->AddItem( "Save As", "img/menu/save.png", "Ctrl + Shift + S" )->SetAction( this, &ThisClass::SaveAsDocument );

		pRoot->GetMenu()->AddItem( "Close", "img/menu/close.png" )->SetAction( this, &ThisClass::CloseDocument );
	}
}

void DesignerBase::CreateToolBar()
{
	Controls::ToolBarStrip* pStrip = new Controls::ToolBarStrip( this );
	pStrip->Dock( Pos::Top );

	pStrip->Add( "New Document", "img/menu/new.png" )->onPress.Add( this, &ThisClass::NewDocument );

	pStrip->Add( "Open", "img/menu/open.png" )->onPress.Add( this, &ThisClass::OpenDocument );
	pStrip->Add( "Save", "img/menu/save.png" )->onPress.Add( this, &ThisClass::SaveDocument );

	// splitter

	pStrip->Add( "Delete", "img/menu/delete.png" )->onPress.Add( this, &ThisClass::DeleteSelected );
	pStrip->Add( "Send Back", "img/menu/back.png" )->onPress.Add( this, &ThisClass::SendBack );
	pStrip->Add( "Bring Forward", "img/menu/forward.png" )->onPress.Add( this, &ThisClass::BringForward );
}

void DesignerBase::CreateControlToolbox()
{
	ControlToolbox* pControlBox = new ControlToolbox( this );
	pControlBox->Dock( Pos::Left );
}

void DesignerBase::CreateDocumentHolder()
{
	m_DocumentHolder = new Controls::TabControl( this );
	m_DocumentHolder->Dock( Pos::Fill );
	m_DocumentHolder->SetMargin( Margin( -1, 2, -1, -1 ) );
}

void DesignerBase::NewDocument()
{
	Controls::TabButton* pButton = m_DocumentHolder->AddPage( L"Untitled Design" );
	pButton->SetImage( "img/document_normal.png" );

	Document* doc = new Document( pButton->GetPage(), "Document" );
	doc->Initialize( pButton );

	pButton->OnPress();
}

void DesignerBase::CloseDocument()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->DelayedDelete();

	Controls::TabButton* pButton = m_DocumentHolder->GetCurrentButton();
	if ( !pButton ) return;

	m_DocumentHolder->RemovePage( pButton );
	pButton->DelayedDelete();
}

void DesignerBase::OpenDocument()
{
	Gwen::Dialogs::FileOpen( true, "", "", "Gwen Designer File|*.gwen", this, &ThisClass::DoOpenDocument );
}

void DesignerBase::DoOpenDocument( Event::Info info )
{
	Controls::TabButton* pButton = m_DocumentHolder->AddPage( L"Loaded" );
	pButton->SetImage( "img/document_normal.png" );

	Document* doc = new Document( pButton->GetPage(), "Document" );
	doc->Initialize( pButton );
	doc->LoadFromFile( info.String.Get(), ImportExport::Find( "Designer" ) );

	pButton->OnPress();
}

void DesignerBase::SaveDocument()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->DoSave( ImportExport::Find( "Designer" ) );
}

void DesignerBase::SaveAsDocument()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->DoSaveAs( ImportExport::Find( "Designer" ) );
}

Document* DesignerBase::CurrentDocument()
{
	Controls::TabButton* pButton = m_DocumentHolder->GetCurrentButton();
	if ( !pButton ) return NULL;

	Document* doc = gwen_cast<Document>(pButton->GetPage()->FindChildByName( "Document" ));
	if ( !doc ) return NULL;

	return doc;
}

void DesignerBase::DeleteSelected()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->Command( "delete" );
}

void DesignerBase::SendBack()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->Command( "sendback" );
}

void DesignerBase::BringForward()
{
	Document* doc = CurrentDocument();
	if ( !doc ) return;

	doc->Command( "bringforward" );
}