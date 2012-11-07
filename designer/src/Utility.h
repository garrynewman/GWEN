

inline Gwen::Controls::Base* FindParentControlFactoryControl( Gwen::Controls::Base* pControl )
{
	while ( pControl )
	{
		if ( pControl->UserData.Exists( "ControlFactory" ) )
		{
			return pControl;
		}

		pControl = pControl->GetParent();
	}

	return NULL;
}