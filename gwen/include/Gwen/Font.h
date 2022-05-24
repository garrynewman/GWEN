/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_FONT_H
#define GWEN_FONT_H

#include <string>
#include <list>

#include "Gwen/BaseRender.h"

namespace Gwen
{

	struct Font
	{
		typedef std::list<Font*>	List;

		Font()
		{
			data = NULL;
			facename = L"Arial";
			size = 10;
			realsize = 0;
			pixel_size = -1;
			bold = false;
		}

		UnicodeString	facename;
		
		// Desired font size in non-dpi corrected pixels
		float size;
		
		// True desired size in pixels. If > 0, we use this instead of size
		float pixel_size;
		
		bool  bold;

		// This should be set by the renderer
		// if it tries to use a font where it's
		// NULL.
		void*	data;

		// This is the real font size, after it's
		// been scaled by Render->Scale()
		float realsize;

	};

} //namespace Gwen
#endif
