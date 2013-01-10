
#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls.h"
#include "Gwen/Util/ImportExport.h"

using namespace Gwen;

class DocumentCanvas;
class Hierarchy;

class Document : public Controls::Base 
{
	GWEN_CONTROL( Document, Controls::Base );

		virtual void Initialize( Controls::TabButton* pTab );

		virtual void DoSaveAs( ImportExport::Base* exporter );
		virtual void DoSave( ImportExport::Base* exporter );
		virtual void LoadFromFile( const Gwen::String& str, ImportExport::Base* exporter );
		
		virtual void Command( const Gwen::String& str );

	protected:

		void DoSaveFromDialog( Event::Info info );
		void OnHierachyChanged( Event::Info info );

		Controls::TabButton*	m_pTab;
		DocumentCanvas*			m_pCanvas;
		Hierarchy*				m_pHierarchy;

		Gwen::String			m_strFilename;

		ImportExport::Base*		m_Exporter;
		ImportExport::Base*		m_Importer;
};