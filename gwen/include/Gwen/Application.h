/*
	GWEN
	Copyright (c) 2022 Matthew B.
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_APPLICATION_H
#define GWEN_APPLICATION_H

#include "Gwen/Controls/Base.h"
#include <Gwen/Controls/WindowCanvas.h>
#include <Gwen/Platform.h>
#include <Gwen/Renderers/OpenGL.h>
#include <Gwen/Skins/TexturedBase.h>

namespace Gwen
{

class Application;
extern Application* gApplication;

// note right now this only supports opengl backends
// could maybe template this on the renderer?
class Application
{
	std::string default_font_ = "Segoe UI";
	double default_font_size_ = 1.0;
	std::string skin_ = "DefaultSkin.png";
	
	std::vector<Gwen::Controls::WindowCanvas*> canvases_;
	
public:

	Application();
	
	~Application()
	{
		CloseAllWindows();
		gApplication = 0;
	}
	
	void SetDefaultFont(const std::string& font, double size)
	{
		default_font_ = font;
		default_font_size_ = size;
	}
	
	void SetSkin(const std::string& texture)
	{
		skin_ = texture;
	}
	
	Gwen::Controls::Base* AddWindow(const std::string& title, int w = 1000, int h = 700, int x = -1, int y = -1)
	{
		Gwen::Renderer::OpenGL* renderer = new Gwen::Renderer::OpenGL();

		auto skin = new Gwen::Skin::TexturedBase(renderer);

		Gwen::Controls::WindowCanvas* window_canvas = new Gwen::Controls::WindowCanvas(x, y, w, h, skin, title);
		window_canvas->SetSizable(true);

		skin->Init(skin_);
		skin->SetDefaultFont(L"Segoe UI", default_font_size_);
		
		canvases_.push_back(window_canvas);
		
		return window_canvas;
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

	// Runs the application loop, returns when program is killed
	bool Spin()
	{
		while (SpinOnce())
		{
			// sleep extra when we are in the background
			bool on_top = false;
			for (const auto& canv: canvases_)
			{
				if (canv->IsOnTop())
				{
					on_top = true;
				}
			}
			
			if (!on_top)
			{
				Gwen::Platform::WaitForEvent();
			}
		}
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
};
}
#endif
