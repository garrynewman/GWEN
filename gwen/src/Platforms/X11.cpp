/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/Macros.h"
#include "Gwen/Platform.h"

#if !defined(GWEN_ALLEGRO_PLATFORM) 
//!defined(_WIN32) && !defined(GWEN_ALLEGRO_PLATFORM)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "Gwen/Input/X11.h"

#include <time.h>

#include <unistd.h>

static Gwen::UnicodeString gs_ClipboardEmulator;
Display* x11_display;
GLXFBConfig global_bestFbc;

static Gwen::Input::X11 GwenInput;

void Gwen::Platform::Sleep( unsigned int iMS )
{
	// TODO.
    usleep(iMS*1000);
}

void Gwen::Platform::SetCursor( unsigned char iCursor )
{
	// No platform independent way to do this
}

Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
	return gs_ClipboardEmulator;
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString & str )
{
	gs_ClipboardEmulator = str;
	return true;
}

float Gwen::Platform::GetTimeInSeconds()
{
	float fSeconds = ( float ) clock() / ( float ) CLOCKS_PER_SEC;
	return fSeconds;
}

bool Gwen::Platform::FileOpen( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	// No platform independent way to do this.
	// Ideally you would open a system dialog here
	return false;
}

bool Gwen::Platform::FileSave( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	// No platform independent way to do this.
	// Ideally you would open a system dialog here
	return false;
}

bool Gwen::Platform::FolderOpen( const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	return false;
}

GWEN_EXPORT void* Gwen::Platform::CreatePlatformWindow( int x, int y, int w, int h, const Gwen::String & strWindowTitle, Gwen::Renderer::Base* renderer)
{
  Display *display = XOpenDisplay(NULL);

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

  printf( "Getting matching framebuffer configs\n" );
  int fbcount;
  GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
  if (!fbc)
  {
    printf( "Failed to retrieve a framebuffer config\n" );
    exit(1);
  }
  printf( "Found %d matching FB configs.\n", fbcount );

  // Pick the FB config/visual with the most samples per pixel
  printf( "Getting XVisualInfos\n" );
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
      
      printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
              " SAMPLES = %d DEPTH = %d\n", 
              i, vi -> visualid, samp_buf, samples, vi->depth );

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
  printf( "Chosen visual ID = 0x%x\n", vi->visualid );

  printf( "Creating colormap\n" );
  XSetWindowAttributes swa;
  Colormap cmap;
  swa.colormap = cmap = XCreateColormap( display,
                                         RootWindow( display, vi->screen ), 
                                         vi->visual, AllocNone );
  swa.background_pixmap = None ;
  swa.border_pixel      = 0;
  swa.event_mask        = StructureNotifyMask;

  printf( "Creating window\n" );
  Window win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                              x, y, w, h, 0, vi->depth, InputOutput, 
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );

  // Done with the visual info data
  XFree( vi );

  XStoreName( display, win, "GL 3.0 Window" );

  printf( "Mapping window\n" );
  XMapWindow( display, win );

  XSelectInput(display, win, ButtonPressMask|ButtonReleaseMask|KeyPressMask|ExposureMask|PointerMotionMask|StructureNotifyMask);

  //glClearColor( 0, 0.5, 1, 1 );
  //glClear( GL_COLOR_BUFFER_BIT );
  //glXSwapBuffers ( display, win );

	return (void*)win;
}

void Gwen::Platform::DestroyPlatformWindow( void* pPtr )
{
    XDestroyWindow(x11_display, (Window)pPtr);
}

void Gwen::Platform::MessagePump( void* pWindow, Gwen::Controls::WindowCanvas* ptarget )
{
    GwenInput.Initialize( ptarget );

    XEvent event;
    while (XPending(x11_display))
    {
        XNextEvent(x11_display, &event);

        if (event.type == ConfigureNotify)
        {
            printf("Width %i Height %i \n", event.xconfigure.width, event.xconfigure.height);
            ptarget->GetSkin()->GetRender()->ResizedContext( ptarget, event.xconfigure.width, event.xconfigure.height );
        }
        if (event.type == Expose && event.xexpose.count == 0)
        {
            ptarget->Redraw();
        }

        GwenInput.ProcessMessage(event);
    }
}

void Gwen::Platform::SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h )
{
    XMoveResizeWindow(x11_display, (Window)pPtr, x, y, w, h);
}

void Gwen::Platform::SetWindowMaximized( void* pPtr, bool bMax, Gwen::Point & pNewPos, Gwen::Point & pNewSize )
{
}

void Gwen::Platform::SetWindowMinimized( void* pPtr, bool bMinimized )
{
}

bool Gwen::Platform::HasFocusPlatformWindow( void* pPtr )
{
	return true;
}

void Gwen::Platform::GetDesktopSize( int & w, int & h )
{
	w = 1024;
	h = 768;
}

void Gwen::Platform::GetCursorPos( Gwen::Point & po )
{
}

#endif // ndef WIN32
