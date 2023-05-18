/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_CONTROLS_LAYOUT_TABLE_H
#define GWEN_CONTROLS_LAYOUT_TABLE_H

#include "Gwen/Controls/Button.h"
#include "Gwen/Utility.h"

namespace Gwen
{
	namespace Controls
	{
		namespace Layout
		{
			class Table;

			class GWEN_EXPORT TableRow : public Base
			{
					static const int MaxColumns = 16;

					GWEN_CONTROL_INLINE( TableRow, Base )
					{
						SetEven( false );

						for ( int i = 0; i < MaxColumns; i++ )
						{ m_Columns[i] = NULL; }

						m_ColumnCount = 0;
					}

					void SetColumnCount( int iCount )
					{
						if ( iCount == m_ColumnCount ) { return; }

						if ( iCount >= MaxColumns )
						{ m_ColumnCount = MaxColumns; }

						for ( int i = 0; i < MaxColumns; i++ )
						{
							if ( i < iCount )
							{
								if ( !m_Columns[i] )
								{
									m_Columns[i] = new Label( this );
									m_Columns[i]->Dock( Pos::Left );
									m_Columns[i]->SetPadding( Padding( 3, 3, 3, 3 ) );
								}
							}
							else if ( m_Columns[i] )
							{
								m_Columns[i]->DelayedDelete();
								m_Columns[i] = NULL;
							}

							m_ColumnCount = iCount;
						}
					}

					void SetColumnWidth( int i, int iWidth )
					{
						if ( !m_Columns[i] ) { return; }

						if ( m_Columns[i]->Width() == iWidth ) { return; }

						m_Columns[i]->SetWidth( iWidth );
					}

					void SetCellText( int i, const TextObject & strString )
					{
						if ( !m_Columns[i] ) { return; }

						m_Columns[i]->SetText( strString );
					}

					void SetCellContents( int i, Base* pControl, bool bEnableMouseInput = false )
					{
						if ( !m_Columns[i] ) { return; }

						pControl->SetParent( m_Columns[i] );
						m_Columns[i]->SetMouseInputEnabled( bEnableMouseInput );
					}

					Label* GetCellContents( int i )
					{
						return m_Columns[i];
					}

					void SizeToContents()
					{
						int iHeight = 0;

						for ( int i = 0; i < m_ColumnCount; i++ )
						{
							if ( !m_Columns[i] ) { continue; }

							// Note, more than 1 child here, because the
							// label has a child built in ( The Text )
							if ( m_Columns[i]->NumChildren() > 1 )
							{
								m_Columns[i]->SizeToChildren();
							}
							else
							{
								m_Columns[i]->SizeToContents();
							}

							iHeight = Utility::Max( iHeight, m_Columns[i]->Height() );
						}

						SetHeight( iHeight );
					}

					void SetTextColor( const Gwen::Color & color )
					{
						for ( int i = 0; i < m_ColumnCount; i++ )
						{
							if ( !m_Columns[i] ) { continue; }

							m_Columns[i]->SetTextColor( color );
						}
					}

					//You might hate this. Actually I know you will
					virtual const TextObject & GetText( int i )
					{
						return m_Columns[i]->GetText();
					}
					virtual void SetSelected( bool /*b*/ ) {}

					//
					// This is sometimes called by derivatives.
					//
					Gwen::Event::Caller	onRowSelected;

					virtual bool GetEven() { return m_bEvenRow; }
					virtual void SetEven( bool b ) { m_bEvenRow = b; }

				private:

					bool	m_bEvenRow;
					int		m_ColumnCount;
					Label*	m_Columns[MaxColumns];

					friend class Table;


			};

			class GWEN_EXPORT Table : public Base
			{
				public:

					GWEN_CONTROL_INLINE( Table, Base )
					{
						m_iColumnCount = 1;
						m_iDefaultRowHeight = 22;

						for ( int i = 0; i < TableRow::MaxColumns; i++ )
						{
							m_ColumnWidth[i] = 0;
						}

						m_bSizeToContents = false;
					}

					void SetColumnCount( int i )
					{
						if ( m_iColumnCount == i ) { return; }

						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							TableRow* pRow = gwen_cast<TableRow> ( *it );

							if ( !pRow ) { continue; }

							pRow->SetColumnCount( i );
						}

						m_iColumnCount = i;
					}

					void SetColumnWidth( int i, int iWidth )
					{
						if ( m_ColumnWidth[i] == iWidth ) { return; }

						m_ColumnWidth[i] = iWidth;
						Invalidate();
					}

					TableRow* AddRow()
					{
						TableRow* row = new TableRow( this );
						AddRow( row );
						return row;
					}

					void AddRow( TableRow* pRow )
					{
						pRow->SetParent( this );
						pRow->SetColumnCount( m_iColumnCount );
						pRow->SetHeight( m_iDefaultRowHeight );
						pRow->Dock( Pos::Top );
						Invalidate();
					}

					TableRow* GetRow( int i )
					{
						return gwen_cast<TableRow> ( GetChild( i ) );
					}

					unsigned int RowCount( int i )
					{
						return NumChildren();
					}

					void Remove( TableRow* pRow )
					{
						pRow->DelayedDelete();
					}

					void Clear()
					{
						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							TableRow* pRow = gwen_cast<TableRow> ( *it );

							if ( !pRow ) { continue; }

							Remove( pRow );
						}
					}

					void Layout( Skin::Base* skin ) override
					{
						BaseClass::Layout( skin );

						if ( m_bSizeToContents )
						{
							DoSizeToContents();
						}

						int iSizeRemainder = Width();
						int iAutoSizeColumns = 0;

						for ( int i = 0; i < TableRow::MaxColumns && i < m_iColumnCount; i++ )
						{
							iSizeRemainder -= m_ColumnWidth[i];

							if ( m_ColumnWidth[i] == 0 ) { iAutoSizeColumns++; }
						}

						if ( iAutoSizeColumns > 1 ) { iSizeRemainder /= iAutoSizeColumns; }

						bool bEven = false;

						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							TableRow* pRow = gwen_cast<TableRow> ( *it );

							if ( !pRow ) { continue; }

							pRow->SizeToContents();
							pRow->SetEven( bEven );
							bEven = !bEven;

							for ( int i = 0; i < TableRow::MaxColumns && i < m_iColumnCount; i++ )
							{
								if ( m_ColumnWidth[i] == 0 )
								{ pRow->SetColumnWidth( i, iSizeRemainder ); }
								else
								{ pRow->SetColumnWidth( i, m_ColumnWidth[i] ); }
							}
						}

						InvalidateParent();
					}

					void PostLayout( Skin::Base* /*skin*/ ) override
					{
						if ( m_bSizeToContents )
						{
							SizeToChildren();
							m_bSizeToContents = false;
						}
					}

					void SizeToContents()
					{
						m_bSizeToContents = true;
						Invalidate();
					}

					void DoSizeToContents()
					{
						for ( int i = 0; i < TableRow::MaxColumns; i++ )
						{
							m_ColumnWidth[i] = 10;
						}

						for ( Base::List::iterator it = Children.begin(); it != Children.end(); ++it )
						{
							TableRow* pRow = gwen_cast<TableRow> ( *it );

							if ( !pRow ) { continue; }

							pRow->SizeToContents();

							for ( int i = 0; i < TableRow::MaxColumns; i++ )
							{
								if ( pRow->m_Columns[i] )
								{
									m_ColumnWidth[i] = Utility::Max( m_ColumnWidth[i], pRow->m_Columns[i]->Width() );
								}
							}

							//iBottom += pRow->Height();
						}

						InvalidateParent();
					}

				private:

					bool	m_bSizeToContents;
					int		m_iColumnCount;
					int		m_iDefaultRowHeight;

					int		m_ColumnWidth[ TableRow::MaxColumns ];
			};
		}
	}
}
#endif
