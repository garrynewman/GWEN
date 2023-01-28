/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/


#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/ScrollControl.h"
#include "Gwen/InputHandler.h"

using namespace Gwen;
using namespace Gwen::Controls;

class ListBoxRow : public Layout::TableRow
{
		GWEN_CONTROL_INLINE( ListBoxRow, Layout::TableRow )
		{
			SetMouseInputEnabled( true );
			SetSelected( false );
		}

		void Render( Skin::Base* skin )
		{
			skin->DrawListBoxLine( this, IsSelected(), GetEven(), IsHovered() );
		}

		bool IsSelected() const
		{
			return m_bSelected;
		}

		void DoSelect()
		{
			SetSelected( true );
			onRowSelected.Call( this );
			Redraw();
		}

		void OnMouseClickLeft( int /*x*/, int /*y*/, bool bDown )
		{
			if ( bDown )
			{
				DoSelect();
			}
		}

		virtual bool ShouldRedrawOnHover() { return true; }

		void SetSelected( bool b )
		{
			m_bSelected = b;

			if ( b )
			{ SetTextColor( GetSkin()->Colors.Label.Bright ); }
			else
			{ SetTextColor( GetSkin()->Colors.Label.Default ); }
		}

	private:

		bool			m_bSelected;

};

GWEN_CONTROL_CONSTRUCTOR( ListBox )
{
	SetScroll( false, true );
	SetAutoHideBars( true );
	SetMargin( Margin( 1, 1, 1, 1 ) );
	m_InnerPanel->SetPadding( Padding( 2, 2, 2, 2 ) );
	m_Table = new Controls::Layout::Table( this );
	m_Table->SetColumnCount( 1 );
	m_bMultiSelect = false;
}

Layout::TableRow* ListBox::AddItem( const TextObject & strLabel, const String & strName )
{
	ListBoxRow* pRow = new ListBoxRow( this );
	m_Table->AddRow( pRow );
	pRow->SetCellText( 0, strLabel );
	pRow->SetName( strName );
	pRow->onRowSelected.Add( this, &ListBox::OnRowSelected );
	return pRow;
}

void ListBox::RemoveItem( Layout::TableRow* row )
{
	ListBox::Rows::iterator it = std::find( m_SelectedRows.begin(), m_SelectedRows.end(), row );
	if( it != m_SelectedRows.end() ) {
		m_SelectedRows.erase( it );
	}
	m_Table->Remove( row );
}

void ListBox::Render( Skin::Base* skin )
{
	skin->DrawListBox( this );
}

void ListBox::Layout( Skin::Base* skin )
{
	BaseClass::Layout( skin );
	const Gwen::Rect & inner = m_InnerPanel->GetInnerBounds();
	m_Table->SetPos( inner.x, inner.y );
	m_Table->SetWidth( inner.w );
	m_Table->SizeToChildren( false, true );
	BaseClass::Layout( skin );
}

void ListBox::UnselectAll()
{
	std::list<Layout::TableRow*>::iterator it = m_SelectedRows.begin();

	while ( it != m_SelectedRows.end() )
	{
		ListBoxRow* pRow = static_cast<ListBoxRow*>( *it );
		it = m_SelectedRows.erase( it );
		pRow->SetSelected( false );
	}
}

void ListBox::OnRowSelected( Base* pControl )
{
	bool bClear = !Gwen::Input::IsShiftDown();

	if ( !AllowMultiSelect() ) { bClear = true; }

	SetSelectedRow( pControl, bClear );
}

Layout::TableRow* ListBox::GetSelectedRow()
{
	if ( m_SelectedRows.empty() ) { return NULL; }

	return *m_SelectedRows.begin();
}

Gwen::String ListBox::GetSelectedRowName()
{
	Layout::TableRow* row = GetSelectedRow();

	if ( !row ) { return ""; }

	return row->GetName();
}

void ListBox::Clear()
{
	UnselectAll();
	m_Table->Clear();
}

void ListBox::SetSelectedRow( Gwen::Controls::Base* pControl, bool bClearOthers )
{
	if ( bClearOthers )
	{ UnselectAll(); }

	ListBoxRow* pRow = gwen_cast<ListBoxRow> ( pControl );

	if ( !pRow ) { return; }

	// TODO: make sure this is one of our rows!
	pRow->SetSelected( true );
	m_SelectedRows.push_back( pRow );
	onRowSelected.Call( this );
}



void ListBox::SelectByString( const TextObject & strName, bool bClearOthers )
{
	if ( bClearOthers )
	{ UnselectAll(); }

	Base::List & children = m_Table->GetChildren();

	for ( Base::List::iterator iter = children.begin(); iter != children.end(); ++iter )
	{
		ListBoxRow* pChild = gwen_cast<ListBoxRow> ( *iter );

		if ( !pChild ) { continue; }

		if ( Utility::Strings::Wildcard( strName, pChild->GetText( 0 ) ) )
		{ SetSelectedRow( pChild, false ); }
	}
}

bool ListBox::OnKeyDown( bool bDown )
{
	if ( bDown )
	{
		Base::List & children = m_Table->Children;
		Base::List::const_iterator begin = children.begin();
		Base::List::const_iterator end = children.end();
		Controls::Base* sel_row = GetSelectedRow();

		if ( sel_row == NULL && !children.empty() ) // no user selection yet, so select first element
		{ sel_row = children.front(); }

		Base::List::const_iterator result = std::find( begin, end, sel_row );

		if ( result != end )
		{
			Base::List::const_iterator next = result;
			++next;

			if ( next != end )
			{ result = next; }

			ListBoxRow* pRow = gwen_cast<ListBoxRow> ( *result );

			if ( pRow )
			{
				pRow->DoSelect();
				Controls::VerticalScrollBar* pScroll = gwen_cast<Controls::VerticalScrollBar> ( m_VerticalScrollBar );

				if ( pScroll ) { pScroll->NudgeDown( this ); }

				Redraw();
			}
		}
	}

	return true;
}

bool ListBox::OnKeyUp( bool bDown )
{
	if ( bDown )
	{
		Base::List & children = m_Table->Children;
		Base::List::const_iterator begin = children.begin();
		Base::List::const_iterator end = children.end();
		Controls::Base* sel_row = GetSelectedRow();

		if ( sel_row == NULL && !children.empty() ) // no user selection yet, so select first element
		{ sel_row = children.front(); }

		Base::List::const_iterator result = std::find( begin, end, sel_row );

		if ( result != end )
		{
			if ( result != begin )
			{ --result; }

			ListBoxRow* pRow = gwen_cast<ListBoxRow> ( *result );

			if ( pRow )
			{
				pRow->DoSelect();
				Controls::VerticalScrollBar* pScroll = gwen_cast<Controls::VerticalScrollBar> ( m_VerticalScrollBar );

				if ( pScroll ) { pScroll->NudgeUp( this ); }

				Redraw();
			}
		}
	}

	return true;
}
