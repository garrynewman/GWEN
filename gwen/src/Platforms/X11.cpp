/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/Macros.h"
#include "Gwen/Platform.h"

#ifndef GWEN_ALLEGRO_PLATFORM
//!defined(_WIN32) && !defined(GWEN_ALLEGRO_PLATFORM)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/cursorfont.h>
#include <GL/gl.h>

#include <GL/glx.h>
#include <sys/time.h>

#include "Gwen/Input/X11.h"

#include <time.h>

#include <unistd.h>

#include <cmath>

#include "portable-file-dialogs.h"

Display* x11_display = 0;
Window x11_window;// the current active window, kinda a terrible hack which breaks thread safety
GLXFBConfig global_bestFbc;

static Gwen::Input::X11 GwenInput;

void Gwen::Platform::Sleep( unsigned int iMS )
{
    usleep(iMS*1000);
}

void Gwen::Platform::SetCursor( unsigned char iCursor )
{
    int cursor_id = 0;
    if (iCursor == Gwen::CursorType::SizeNS)
    {
        cursor_id = XC_sb_v_double_arrow;
    }
    else if (iCursor == Gwen::CursorType::SizeWE)
    {
        cursor_id = XC_sb_h_double_arrow;
    }
    else if (iCursor == Gwen::CursorType::SizeNWSE)
    {
        cursor_id = XC_bottom_right_corner;
    }
    else if (iCursor == Gwen::CursorType::SizeNESW)
    {
        cursor_id = XC_bottom_left_corner;
    }
    else if (iCursor == Gwen::CursorType::SizeAll)
    {
        cursor_id = XC_fleur;
    }
    else if (iCursor == Gwen::CursorType::Finger)
    {
        cursor_id = XC_hand1; 
    }
    else if (iCursor == Gwen::CursorType::No)
    {
        cursor_id = XC_X_cursor; 
    }
    else if (iCursor == Gwen::CursorType::Beam)
    {
        cursor_id = XC_xterm;
    }
    else// normal
    {
        XUndefineCursor(x11_display, x11_window);
        return;
    }
	
    Cursor c = XCreateFontCursor(x11_display, cursor_id);
    XDefineCursor(x11_display, x11_window, c);// todo need to get the correct window
}

static std::string copy_buffer;
Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
	if (copy_buffer.length())
	{
		return Gwen::Utility::StringToUnicode(copy_buffer);
	}
	
	char *result;
	unsigned long ressize, restail;
	int resbits;
	const char* fmtname = "UTF8_STRING";//"STRING";
	Atom bufid = XInternAtom(x11_display, "CLIPBOARD", False);
	Atom fmtid = XInternAtom(x11_display, fmtname, False);
	Atom propid = XInternAtom(x11_display, "XSEL_DATA", False);
	Atom incrid = XInternAtom(x11_display, "INCR", False);

	XConvertSelection(x11_display, bufid, fmtid, propid, x11_window, CurrentTime);

	XEvent event;
	do
	{
		XNextEvent(x11_display, &event);
	}
	while (event.type != SelectionNotify || event.xselection.selection != bufid);

	if (event.xselection.property)
	{
		XGetWindowProperty(x11_display, x11_window, propid, 0, 500000, False, AnyPropertyType,
		  &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

		if (fmtid == incrid)
		{
			printf("Buffer too large to paste.\n");
		}
		
		// handle parsing any unicode values, which appear to be \\U00000000
		/*const char* buf = new char[ressize];
		int size = 0;
		for (int i = 0; i < ressize; i++)
		{
			// copy, and parse any unicode
			if (
		}
		buf[size] = 0;*/
		Gwen::UnicodeString str = Gwen::Utility::StringToUnicode(result);
		XFree(result);
		return str;
	}
	else // request failed, e.g. owner can't convert to the target format
	{
		return L"";
	}
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString & str )
{
	Atom clip = XInternAtom(x11_display, "CLIPBOARD", False);
	Atom primary = XInternAtom(x11_display, "PRIMARY", False);
		
	copy_buffer = Gwen::Utility::UnicodeToString(str);
	XSetSelectionOwner(x11_display, clip, DefaultRootWindow(x11_display), CurrentTime);
	XSetSelectionOwner(x11_display, primary, DefaultRootWindow(x11_display), CurrentTime);

	return true;
}

static uint64_t start_time = 0;
float Gwen::Platform::GetTimeInSeconds()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	
	uint64_t time_us = tv.tv_sec*1000000 + tv.tv_usec;
	if (start_time == 0)
	{
		start_time = time_us;
	}
	
	uint64_t dt = time_us - start_time;
	float fSeconds = (float)dt/1000000.0f;
	return fSeconds;
}

static std::vector<std::string> split(std::string s, char delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = 1;
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

bool Gwen::Platform::FileOpen( const String & Name, const String & StartPath, const String & Extension,
	Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	std::vector<std::string> extensions = split(Extension, '|');
	auto files = pfd::open_file(Name, StartPath, extensions, pfd::opt::none).result();

	if ( pHandler && fnCallback && files.size() > 0)
	{
		Gwen::Event::Information info;
		info.Control		= NULL;
		info.ControlCaller	= NULL;
		info.String			= files[0];
		( pHandler->*fnCallback )( info );
	}
	return true;
}

bool Gwen::Platform::FileSave( const String & Name, const String & StartPath, const String & Extension,
	Gwen::Event::Handler* pHandler, Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	// split out extensions
	std::vector<std::string> extensions = split(Extension, '|');
	auto file = pfd::save_file(Name, StartPath, extensions, pfd::opt::none).result();

	if ( pHandler && fnCallback && file.length())
	{
		Gwen::Event::Information info;
		info.Control		= NULL;
		info.ControlCaller	= NULL;
		info.String			= file;
		( pHandler->*fnCallback )( info );
	}
    return true;
}

bool Gwen::Platform::FolderOpen( const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	auto folder = pfd::select_folder(Name, StartPath).result();

	if ( pHandler && fnCallback && folder.length())
	{
		Gwen::Event::Information info;
		info.Control		= NULL;
		info.ControlCaller	= NULL;
		info.String			= folder;
		( pHandler->*fnCallback )( info );
	}
	return true;
}

typedef struct
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
} Hints;

#define XA_ATOM ((Atom)4)

static struct { 
    int pip[2];     // extra pipe for event loop
    sig_atomic_t done;
} global;
static int xfd;
static Atom delete_msg;

GWEN_EXPORT void* Gwen::Platform::CreatePlatformWindow( int x, int y, int w, int h, 
	const Gwen::String & strWindowTitle, Gwen::Renderer::Base* renderer, bool is_menu)
{
	Display *display = x11_display ? x11_display : XOpenDisplay(NULL);
	if (!x11_display)
	{
		memset(&global, 0, sizeof(global));
		pipe(global.pip);
		xfd = ConnectionNumber(display);
	}
	//printf("Creating window\n");
	if (!display)
	{
		printf("Failed to open X display\n");
		exit(1);
	}

	x11_display = display;

	// Get a matching FB config
	static int visual_attribs[] =
	{
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
	};

	int glx_major, glx_minor;
 
  // FBConfigs were added in GLX version 1.3.
	if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
       ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
	{
		printf("Invalid GLX version");
		exit(1);
	}

	//printf( "Getting matching framebuffer configs\n" );
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if (!fbc)
	{
		printf( "Failed to retrieve a framebuffer config\n" );
		exit(1);
	}
	//printf( "Found %d matching FB configs.\n", fbcount );

	// Pick the FB config/visual with the most samples per pixel
	//printf( "Getting XVisualInfos\n" );
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for (i=0; i<fbcount; ++i)
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
		if ( vi )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
      
			//printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
			//        " SAMPLES = %d DEPTH = %d\n", 
			//        i, vi -> visualid, samp_buf, samples, vi->depth );

			if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
				best_fbc = i, best_num_samp = samples;
			if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree( vi );
	}

	GLXFBConfig bestFbc = fbc[ 0 ];//best_fbc ];
	global_bestFbc = bestFbc;

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree( fbc );

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
	//printf( "Chosen visual ID = 0x%x\n", vi->visualid );

	//printf( "Creating colormap\n" );
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap( display,
                                         RootWindow( display, vi->screen ), 
                                         vi->visual, AllocNone );
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	//printf( "Creating window\n" );
	Window win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                              x, y, w, h, 0, vi->depth, InputOutput, 
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );

	// Done with the visual info data
	XFree( vi );

	XStoreName( display, win, strWindowTitle.c_str() );
  
  	
  	// Hide borders
  	if (!WindowHasTitleBar() || is_menu)
  	{
		Hints hints;
		hints.flags = 2;
		hints.decorations = 0;
		Atom property = XInternAtom(display,"_MOTIF_WM_HINTS",True);
		XChangeProperty(display,win,property,property,32,PropModeReplace,(unsigned char *)&hints,5);
	}

	// Tell the window manager that we arent a normal window so we dont get added to taskbar
	if (is_menu)
	{
		Atom type_atom = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE_MENU", False);
		Atom wt_atom = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE", False);

		if (wt_atom != None && type_atom != None)
		{
			XChangeProperty(display, win, wt_atom, XA_ATOM, 32, PropModeReplace, (unsigned char *)&type_atom, 1);
		}
	}

	//printf( "Mapping window\n" );
	XMapWindow( display, win );

	XSelectInput(display, win, ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|
		ExposureMask|PointerMotionMask|StructureNotifyMask|FocusChangeMask|PropertyChangeMask);

	x11_window = win;

	// Let us capture close events so we can properly handle them
	Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	delete_msg = wmDeleteMessage;
	XSetWMProtocols(display, win, &wmDeleteMessage, 1);

	// Make sure we get placed in the correct spot
	XMoveResizeWindow(x11_display, win, x, y, w, h);
	
	return (void*)win;
}

void Gwen::Platform::SetWindowTitle(void* pPtr, const Gwen::String & strWindowTitle)
{
	XStoreName( x11_display, (Window)pPtr, strWindowTitle.c_str() );
}

static std::map<Window, Gwen::Controls::WindowCanvas*> canvases;
void Gwen::Platform::DestroyPlatformWindow( void* pPtr )
{
	canvases.erase((Window)pPtr);
    XDestroyWindow(x11_display, (Window)pPtr);
}

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

double _font_scale = 1.0;

double GetDPIInfo()
{
    auto display_2 = XOpenDisplay(0);
    char *resourceString = XResourceManagerString(display_2);
    XrmDatabase db;
    XrmValue value;
    char *type = NULL;
    double dpi = 0.0;

    XrmInitialize(); /* Need to initialize the DB before calling Xrm* functions */

    db = XrmGetStringDatabase(resourceString);

    if (resourceString) {
        //printf("Entire DB:\n%s\n", resourceString);
        if (XrmGetResource(db, "Xft.dpi", "String", &type, &value) == True) {
            if (value.addr) {
                dpi = atof(value.addr);
            }
        }
    }
	XrmDestroyDatabase(db);

    //printf("Font DPI: %f\n", dpi);// this is actually the font scale setting!
    
    // try other method
    int screen = DefaultScreen(display_2);
    
    //printf("Width: %i\n", DisplayWidthMM(display_2, screen));

    
    double real_dpi = DisplayWidth(display_2, screen)/(DisplayWidthMM(display_2, screen)/25.4);
    XCloseDisplay(display_2);
    //printf("Real DPI: %f\n", real_dpi);

    // round to the nearest 0.25 dpi
    real_dpi = std::round(real_dpi/24.0)*24.0;

    int rdpi = real_dpi;
    
    float font_scale = dpi / 96.0;
    if (_font_scale != font_scale)
    {
    	_font_scale = font_scale;
    	for (auto& canv: canvases)
    	{
    		canv.second->SetFontScale(_font_scale);
    	}
    }
    
    return rdpi;
}

void Gwen::Platform::MessagePump( void* pWindow, Gwen::Controls::WindowCanvas* ptarget )
{	
	x11_window = (Window)ptarget->GetWindow();
	
	canvases[(Window)ptarget->GetWindow()] = ptarget;
    
	// handle drag and drop off window in a weird special case
	if (Gwen::Input::IsLeftMouseDown())
	{
		// manually look for mouse up if we are outside the window
		int x, y, lx, ly;
		unsigned int mask;
		Window root, child;
		XQueryPointer(x11_display, x11_window, &root, &child, &x, &y, &lx, &ly, &mask);

		if ((mask & Button1Mask) == 0)
		{
			Gwen::Input::OnMouseButton(0, false);
		}
	}

	XEvent event;
	while (XPending(x11_display))
	{
		XNextEvent(x11_display, &event);
		
		if (event.type == Expose || event.type == PropertyNotify)
		{
			double dpi = GetDPIInfo();
			// sometimes we get spurrious 0 dpi values, ignore those
			if (ptarget->GetDPI() != dpi && dpi > 1.0)
			{
				//printf("%f vs %f\n", ptarget->GetDPI(), dpi);
				// todo, how do I size the window properly?
				float ds = dpi/ptarget->GetDPI();
				ptarget->SetDPI(dpi);
		
				auto pos = ptarget->WindowPosition();
				ptarget->SetPos(pos.x, pos.y);
				ptarget->SetScale(dpi / 96.0);
			}
		}

		// Catch window close requests
		if (event.type == ClientMessage && event.xclient.data.l[0] == delete_msg)
		{
			canvases[event.xclient.window]->InputQuit();
			canvases.erase(event.xclient.window);
			continue;
		}
       	
		// Handle pastes requested by other windows
		if (event.type == SelectionRequest)
		{
			XEvent reply = {0};
			reply.xselection.type = SelectionNotify;
			reply.xselection.requestor = event.xselectionrequest.requestor;
			reply.xselection.selection = event.xselectionrequest.selection;
			reply.xselection.target = event.xselectionrequest.target;
			reply.xselection.time = event.xselectionrequest.time;
			reply.xselection.property = None;
       		
			if (copy_buffer.length())
			{
				Atom xa_targets = XInternAtom(x11_display, "TARGETS", False);
				Atom xa_text = XInternAtom(x11_display, "TEXT", False);
				Atom xa_string = XInternAtom(x11_display, "STRING", False);
				Atom xa_utf8_string = XInternAtom(x11_display, "UTF8_STRING", False);
				// Provide supported types
				if (reply.xselection.target == xa_targets)
				{
					Atom tar_list[4];
					tar_list[0] = xa_targets;
					tar_list[1] = xa_utf8_string;
					tar_list[2] = xa_string;
       				
					reply.xselection.property = event.xselectionrequest.property;
					XChangeProperty(event.xselection.display, event.xselectionrequest.requestor,
       					reply.xselection.property, XInternAtom(x11_display, "ATOM", False), 32, PropModeReplace,
       					(unsigned char*)&tar_list, 3);
				}
				// todo check if they are requesting a supported type
				else// if (reply.xselection.property == xa_text ||
       				//	 reply.xselection.property == xa_string ||
       				//	 reply.xselection.property == xa_utf8_string)// paste
				{
					reply.xselection.property = event.xselectionrequest.property;
					XChangeProperty(x11_display, event.xselectionrequest.requestor,
						reply.xselection.property, reply.xselection.target, 8,
						PropModeReplace, (unsigned char*)copy_buffer.c_str(), copy_buffer.length() + 1);
				}
			}
			XSendEvent(x11_display, event.xselectionrequest.requestor, True, 0, &reply);
			XFlush(x11_display);
			continue;
		}
       	
		if (event.type == SelectionClear)
		{
			copy_buffer.clear();
			continue;
		}
       	
		// process it for _every_ window
		// not that this is correct really...
		for (auto canv: canvases)
		{
			// handle window resizes and whatnot
			if (event.type == ConfigureNotify && event.xconfigure.window == canv.first)
			{
				float scale = canv.second->GetDPIScaling().x;
				canv.second->GetSkin()->GetRender()->ResizedContext( canv.second, event.xconfigure.width, event.xconfigure.height );
				canv.second->OnMove(event.xconfigure.x, event.xconfigure.y);
				canv.second->SetSize(event.xconfigure.width/scale, event.xconfigure.height/scale);// this is kinda weird, but meh
			}
			if ((event.type == Expose && event.xexpose.count == 0) || event.type == FocusOut || event.type == FocusIn)
			{
				if (event.xany.window == canv.first)
				{
					canv.second->Redraw();
				}
			}
		}
		
		GwenInput.ProcessMessage(event);
	}
}

void Gwen::Platform::SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h )
{
	// setting size to <= 0 is an error, clamp at one
	w = std::max(1, w);
	h = std::max(1, h);
	XMoveResizeWindow(x11_display, (Window)pPtr, x, y, w, h);
}

void Gwen::Platform::SetWindowMaximized( void* pPtr, bool bMax, Gwen::Point & pNewPos, Gwen::Point & pNewSize )
{
	XEvent xev;
	Atom wm_state  =  XInternAtom(x11_display, "_NET_WM_STATE", False);
	Atom max_horz  =  XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom max_vert  =  XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = (Window)pPtr;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bMax ? 1 : 0;//_NET_WM_STATE_ADD, _NET_WM_STATE_REMOVE
	xev.xclient.data.l[1] = max_horz;
	xev.xclient.data.l[2] = max_vert;

	XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	
	if (!bMax)
	{
		// Because tiling WMs are weird, we must maximize then unmaximize to actually undock
		memset(&xev, 0, sizeof(xev));
		xev.type = ClientMessage;
		xev.xclient.window = (Window)pPtr;
		xev.xclient.message_type = wm_state;
		xev.xclient.format = 32;
		xev.xclient.data.l[0] = 1;//_NET_WM_STATE_ADD
		xev.xclient.data.l[1] = max_vert;

		XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
		XFlush(x11_display);
	
		memset(&xev, 0, sizeof(xev));
		xev.type = ClientMessage;
		xev.xclient.window = (Window)pPtr;
		xev.xclient.message_type = wm_state;
		xev.xclient.format = 32;
		xev.xclient.data.l[0] = 0;//_NET_WM_STATE_REMOVE
		xev.xclient.data.l[1] = max_vert;

		XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
		XFlush(x11_display);
	}
	
	// Give a bit of time for the resize so we get an accurate result below
	Sleep(50);

	Window root;
	unsigned int width, height, border_width, depth;
	int left, top;
	XGetGeometry(x11_display, (Window)pPtr, &root, &left, &top, &width, &height, &border_width, &depth);
	pNewPos.x = left;
	pNewPos.y = top;
	pNewSize.x = width;
	pNewSize.y = height;
}

void Gwen::Platform::SetWindowMinimized( void* pPtr, bool bMinimized )
{
	Atom prop = XInternAtom(x11_display, "WM_CHANGE_STATE", False);
	if (prop == None)
		return;

	XClientMessageEvent ev;
	ev.type = ClientMessage;
	ev.window = (Window)pPtr;
	ev.message_type = prop;
	ev.format = 32;
	ev.data.l[0] = bMinimized ? IconicState : NormalState;
	XSendEvent(x11_display, DefaultRootWindow(x11_display), False,
            SubstructureRedirectMask|SubstructureNotifyMask,
            (XEvent *)&ev);
}

bool Gwen::Platform::HasFocusPlatformWindow( void* pPtr )
{
	Window focused;
	int revert_to;
	XGetInputFocus(x11_display, &focused, &revert_to);
	
	return focused == (Window)pPtr;
}

void Gwen::Platform::GetDesktopSize( int & w, int & h )
{
	Display* display = x11_display;
	bool opened_display = false;
	if (!display)
	{
		display = XOpenDisplay(NULL);
		opened_display = true;
	}

	Screen* screen = XDefaultScreenOfDisplay(display);
	
	w = XWidthOfScreen(screen);
	h = XHeightOfScreen(screen);

	if (opened_display)
	{
		XCloseDisplay(display);
	}
}

void Gwen::Platform::GetCursorPos( Gwen::Point & po )
{
	int x, y, lx, ly;
	unsigned int mask;
	Window root, child;
	XQueryPointer(x11_display, x11_window, &root, &child, &x, &y, &lx, &ly, &mask);
	po.x = x;
	po.y = y;
}

bool Gwen::Platform::WindowHasTitleBar()
{
	return false;
}

void Gwen::Platform::InterruptWait()
{
	// Wake up select below in WaitForEvent
	char buf = 0;
	write(global.pip[1], &buf, 1);
}

void Gwen::Platform::WaitForEvent(int delay_ms)
{
	// This is the naieve method which basically gets stuck forever
	//XEvent ev;
	//XPeekEvent(x11_display, &ev);
	
	if (XEventsQueued(x11_display, QueuedAlready))
	{
		return;
	}
	
	// initialize the set to scan
	static fd_set set_read;
	FD_ZERO(&set_read);
	FD_SET(xfd, &set_read);
	FD_SET(global.pip[0], &set_read);
	
	struct timeval tv;
	tv.tv_sec = delay_ms/1000;
	tv.tv_usec = (delay_ms%1000)*1000;
	
	// block on X11 and pipe so we can interrupt this
	int max_fd = xfd > global.pip[0] ? xfd : global.pip[0];
	if (delay_ms > 0)
	{
		select(max_fd+1, &set_read, NULL, NULL, &tv);
	}
	else
	{
		select(max_fd+1, &set_read, NULL, NULL, NULL);
	}
    
	// Read to clear the interrupt if it was the cause of our wakeup
	if (FD_ISSET(global.pip[0], &set_read))
	{
		char buf[50];
		read(global.pip[0], buf, 50);
	}
}

void Gwen::Platform::SetWindowMinimumSize( void* pPtr, int min_width, int min_height)
{
	XSizeHints hints;
	memset(&hints, 0, sizeof(XSizeHints));
	hints.min_width = min_width;
	hints.min_height = min_height;
	hints.flags = PMinSize;
	XSetWMNormalHints(x11_display, (Window)pPtr, &hints);
	XSetWMSizeHints(x11_display, (Window)pPtr, &hints, PMinSize);
}

#define XA_ATOM ((Atom) 4)

bool Gwen::Platform::IsWindowMaximized( void* pPtr )
{
	Atom property = XInternAtom(x11_display, "_NET_WM_STATE", False);
	Atom type;
	int format;
	unsigned long len;
	unsigned long remaining;
	unsigned char *data = nullptr;
	bool retval = false;

	if (property == None) {
		return false;
	}

	int result = XGetWindowProperty(
			x11_display,
			(Window)pPtr,
			property,
			0,
			1024,
			False,
			XA_ATOM,
			&type,
			&format,
			&len,
			&remaining,
			&data);

	if (result == Success && data) {
		Atom *atoms = (Atom *)data;
		Atom wm_act_max_horz = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		Atom wm_act_max_vert = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		bool found_wm_act_max_horz = false;
		bool found_wm_act_max_vert = false;

		for (uint64_t i = 0; i < len; i++) {
			if (atoms[i] == wm_act_max_horz) {
				found_wm_act_max_horz = true;
			}
			if (atoms[i] == wm_act_max_vert) {
				found_wm_act_max_vert = true;
			}

			if (found_wm_act_max_horz || found_wm_act_max_vert) {
				retval = true;
				break;
			}
		}

		XFree(data);
	}

	return retval;
}

#endif // ndef WIN32
