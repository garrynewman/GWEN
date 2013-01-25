/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_LISTBOX_H
#define GWEN_CONTROLS_LISTBOX_H

#include "Gwen/Gwen.h"
#include "Gwen/Controls/Layout/Table.h"
#include "Gwen/Controls/ScrollControl.h"


namespace Gwen 
{
	namespace Controls
	{
		class ScrollControl;

		class GWEN_EXPORT ListBox : public ScrollControl
		{
			public:
				
				GWEN_CONTROL( ListBox, ScrollControl );

				typedef std::list<Layout::TableRow*> Rows;

				Layout::TableRow* AddItem( const TextObject& strLabel, const String& strName = "" );

				void RemoveItem( Layout::TableRow * row );

				void Render( Skin::Base* skin );
				void Layout( Skin::Base* skin );

				void UnselectAll();			

				void SetAllowMultiSelect( bool bMultiSelect ){ m_bMultiSelect = bMultiSelect; }
				bool AllowMultiSelect() const { return m_bMultiSelect; }

				const ListBox::Rows& GetSelectedRows(){ return m_SelectedRows; }

				virtual Layout::TableRow* GetSelectedRow();
				virtual Gwen::String GetSelectedRowName();

				virtual void SetSelectedRow( Gwen::Controls::Base* pRow, bool bClearOthers = true );
				virtual void SelectByString( const TextObject& string, bool bClearOthers = true );
				
				Gwen::Event::Caller	onRowSelected;

				Controls::Layout::Table* GetTable() { return m_Table; }
				virtual void Clear();

				// Pass through, to embedded table
				void SetColumnCount( int iCount ) { m_Table->SetColumnCount( iCount ); }
				void SetColumnWidth( int iCount, int iSize ) { m_Table->SetColumnWidth( iCount, iSize ); }

			protected:

				
				void OnRowSelected( Base* pControl );
				bool OnKeyDown(bool bDown);
				bool OnKeyUp(bool bDown);
				
				Controls::Layout::Table*		m_Table;
				ListBox::Rows					m_SelectedRows;

				bool m_bMultiSelect;
		};
	}
}
#endif
