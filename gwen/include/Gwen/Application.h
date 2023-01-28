/*
	GWEN
	Copyright (c) 2022 Matthew B.
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_APPLICATION_H
#define GWEN_APPLICATION_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/WindowCanvas.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/Platform.h"
#include "Gwen/Anim.h"

#include "../../resources.h"

#ifndef _WIN32
#include <signal.h>
#endif


namespace Gwen
{

class BaseApplication;
extern BaseApplication* gApplication;

// note right now this only supports opengl backends
// could maybe template this on the renderer?

class BaseApplication
{
public:
	virtual Gwen::Controls::WindowCanvas* AddWindow(const std::string& title, int w, int h, int x = -1, int y = -1, bool is_menu = false) = 0;

	virtual void RequestQuit() = 0;
};

#ifndef _WIN32
void signal_handler(int sig);
#endif

template<class T>
class Application: public BaseApplication
{
	std::wstring default_font_ = L"Segoe UI";
	double default_font_size_ = 10.0;
	std::string skin_ = "DefaultSkin.png";
	
	std::vector<Gwen::Controls::WindowCanvas*> canvases_;
	
public:

	Application()
	{
		if (gApplication)
		{
			printf("Cannot instantiate multiple Gwen::Applications per process...\n");
			throw 7;
		}
		gApplication = this;

#ifndef _WIN32
		signal(SIGINT, signal_handler);
#endif
	}
	
	~Application()
	{
		CloseAllWindows();
		gApplication = 0;
	}
	
	void SetDefaultFont(const std::wstring& font, double size)
	{
		default_font_ = font;
		default_font_size_ = size;
	}
	
	void SetSkin(const std::string& texture)
	{
		skin_ = texture;
	}
	
	void RequestQuit()
	{
		for (auto& canv: canvases_)
		{
			canv->InputQuit();
		}
		Gwen::Platform::InterruptWait();
	}
	
	void CloseAllWindows()
	{
		for (auto& canv: canvases_)
		{
			auto render = canv->GetSkin()->GetRender();
			auto skin = canv->GetSkin();
			delete canv;
			delete skin;
			delete render;
		}
		canvases_.clear();
	}

	// Returns false if the program should exit
	bool Okay()
	{
		return canvases_.size() > 0;// todo
	}
	
	// Runs the application loop once then returns. Returns false if the program should exit
	bool SpinOnce()
	{
		Gwen::Input::OnThink();
		
		for (int i = 0; i < canvases_.size(); i++)
		{
			auto canv = canvases_[i];
			
			if (canv->WantsQuit())
			{
				//remove it
				canvases_.erase(canvases_.begin() + i);
				auto render = canv->GetSkin()->GetRender();
				auto skin = canv->GetSkin();
				delete canv;
				delete skin;
				delete render;

				i--;
				continue;
			}

			canv->DoThink();
		}
		
		return canvases_.size() != 0;
	}

	// Runs the application loop, returns when program is killed
	bool Spin()
	{
		while (SpinOnce())
		{
			for (auto& canv : canvases_)
			{
				if (canv->NeedsRedraw())
				{
					continue;
				}
			}

			// If we dont need a redraw, sleep until we get new input
			if (!Gwen::Anim::HasActiveAnimation())
			{
				Gwen::Platform::WaitForEvent(0);
			}
			else
			{
				// If we have an active anim, update at 60 Hz
				Gwen::Platform::WaitForEvent(16);
			}
		}
		return true;
	}

	Gwen::Controls::WindowCanvas* AddWindow(const std::string& title, int w, int h, int x = -1, int y = -1, bool is_menu = false)
	{
		T* renderer = new T();

		auto skin = new Gwen::Skin::TexturedBase(renderer);// todo parameterize this

		Gwen::Controls::WindowCanvas* window_canvas = new Gwen::Controls::WindowCanvas(x, y, w, h, skin, title, is_menu);
		window_canvas->SetSizable(!is_menu);

		if (FILE* f = fopen(skin_.c_str(), "rb"))
		{
			fclose(f);
		}
		else
		{
			// create the skin from default
			f = fopen(skin_.c_str(), "wb");
			fwrite(DefaultSkin_png, 1, DefaultSkin_png_size, f);
			fclose(f);
			printf("Skin doesnt exist. Creating from default.\n");
		}

		skin->Init(skin_);
		skin->SetDefaultFont(default_font_, default_font_size_);

		canvases_.push_back(window_canvas);

		return window_canvas;
	}
};
}
#endif
