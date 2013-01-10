#include "Properties.h"
#include "DocumentCanvas.h"
#include "Gwen/Util/ControlFactory.h"

GWEN_CONTROL_CONSTRUCTOR( Properties )
{
	SetSize( 200, 200 );

	m_Props = new Controls::PropertyTree( this );
	m_Props->Dock( Pos::Fill );


}

void Properties::WatchCanvas( DocumentCanvas* pCanvas )
{
	m_pCanvas = pCanvas;
	m_pCanvas->onSelectionChanged.Add( this, &ThisClass::OnCanvasSelectionChanged );
	m_pCanvas->onPropertiesChanged.Add( this, &ThisClass::OnPropertiesChanged );

	
	//m_pCanvas->onChildAdded.Add( this, &ThisClass::OnCanvasChildAdded );

}


void Properties::OnCanvasSelectionChanged( Event::Info info )
{
	m_Props->Clear();

	m_SelectedControls = info.ControlList;

	for ( ControlList::List::const_iterator it = m_SelectedControls.list.begin(); it != m_SelectedControls.list.end(); ++it )
	{
		AddPropertiesFromControl( *it, true );
	}

	m_Props->ExpandAll();
}

void Properties::AddPropertiesFromControl( Controls::Base* pControl, bool bAllowDifferent )
{
	if ( !pControl->UserData.Exists( "ControlFactory" ) )
		return;

	ControlFactory::Base* cf = pControl->UserData.Get<ControlFactory::Base*>( "ControlFactory" );

	//
	// Foreach control control factory on this control
	//
	while ( cf )
	{
		Controls::Properties* properties = m_Props->Find( cf->Name() );

		if ( !properties ) properties = m_Props->Add( cf->Name() );

		//
		// And foreach property in that control factory
		//
		ControlFactory::Property::List::const_iterator it = cf->Properties().begin();
		ControlFactory::Property::List::const_iterator itEnd = cf->Properties().end();
		for ( it; it != itEnd; ++it )
		{
			Gwen::String strPropertyName = (*it)->Name();
			Gwen::UnicodeString strValue = (*it)->GetValue( pControl );

			//
			// Add a property row to our control
			//
			Controls::PropertyRow* row = properties->Find( strPropertyName );
			if ( !row )
			{
				if ( (*it)->OptionNum() > 0 )
				{
					//row = properties->Add( strPropertyName, new Gwen::Controls::Property::Checkbox( properties ), strValue );

					Gwen::Controls::Property::ComboBox* dd = new Gwen::Controls::Property::ComboBox( properties );

					for ( int i=0; i<(*it)->OptionNum(); i++ )
					{
						dd->GetComboBox()->AddItem( (*it)->OptionGet(i), Gwen::Utility::UnicodeToString( (*it)->OptionGet(i) ) );
					}

					row = properties->Add( strPropertyName, dd, strValue );
				}
				else
				{	
					row = properties->Add( strPropertyName, strValue );
				}

				row->SetName( (*it)->Name() );
				row->onChange.Add( this, &ThisClass::OnPropertyChanged );
			}
			else
			{
				if ( bAllowDifferent && row->GetProperty()->GetPropertyValue().GetUnicode() != strValue )
				{
					row->GetProperty()->SetPropertyValue( "different" );
				}
				else 
				{
					row->GetProperty()->SetPropertyValue( strValue, false );
				}
			}
		}

		cf = cf->GetBaseFactory();
	}
}

void Properties::OnPropertyChanged( Event::Info info )
{
	for ( ControlList::List::const_iterator it = m_SelectedControls.list.begin(); it != m_SelectedControls.list.end(); ++it )
	{
		Controls::Base* pControl = (*it);
		ControlFactory::Base* cf = pControl->UserData.Get<ControlFactory::Base*>( "ControlFactory" );

		while ( cf )
		{
			cf->SetControlValue( pControl, info.ControlCaller->GetName(), info.String.GetUnicode() );
			cf = cf->GetBaseFactory();
		}
	}
}

void Properties::OnPropertiesChanged( Event::Info info )
{
	m_SelectedControls = info.ControlList;
	
	for ( ControlList::List::const_iterator it = m_SelectedControls.list.begin(); it != m_SelectedControls.list.end(); ++it )
	{
		AddPropertiesFromControl( *it, it != m_SelectedControls.list.begin() );
	}

	m_Props->ExpandAll();
}