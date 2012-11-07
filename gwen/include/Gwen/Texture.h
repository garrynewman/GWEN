/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_TEXTURE_H
#define GWEN_TEXTURE_H

#include <string>

#include "Gwen/BaseRender.h"

namespace Gwen
{
	//
	// Texture
	//
	struct Texture
	{
		typedef std::list<Texture*>		List;

		TextObject	name;
		void*	data;
		bool	failed;
		int		width;
		int		height;
		Gwen::Renderer::Base* acquired; // @rlyeh: this renderer will release any acquired texture

		Texture()
		{
			data = NULL;
			width = 4;
			height = 4;
			failed = false;
			acquired = NULL;
		}

		~Texture()
		{
			if( acquired )
			{
				Release( acquired );  // @rlyeh: free any memory leak
				acquired = NULL;
			}
		}

		void Load( const TextObject& str, Gwen::Renderer::Base* render )
		{
			Gwen::Debug::AssertCheck( acquired == NULL, "Release me first!" ); // @rlyeh: ensure texture has not been acquired previously
			acquired = render;

			name = str;
			Gwen::Debug::AssertCheck( render != NULL, "No renderer!" );
			render->LoadTexture( this );
		}

		void Release( Gwen::Renderer::Base* render )
		{
			Gwen::Debug::AssertCheck( acquired != NULL, "Load me first!" ); // @rlyeh: ensure texture has been acquired previously
			Gwen::Debug::AssertCheck( acquired == render, "Renderer mismatch!" ); // @rlyeh: ensure we're using same renderer to release texture
			acquired = 0;

			render->FreeTexture( this );
		}

		bool FailedToLoad() const
		{
			return failed;
		}
	};

}
#endif
