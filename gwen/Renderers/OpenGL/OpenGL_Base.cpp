
#include "Gwen/Renderers/OpenGL_Base.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <math.h>

#include "GL/glew.h"
#include "FreeImage/FreeImage.h"

#ifndef _WIN32
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#endif

extern Display* x11_display;
extern GLXFBConfig global_bestFbc;

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;
  
  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;

    start = terminator;
  }

  return false;
}


#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

namespace Gwen
{
	namespace Renderer
	{
		OpenGL_Base::OpenGL_Base()
		{
			m_iVertNum = 0;
			m_pContext = NULL;
			::FreeImage_Initialise();

			for ( int i = 0; i < MaxVerts; i++ )
			{
				m_Vertices[ i ].z = 0.5f;
			}
		}

		OpenGL_Base::~OpenGL_Base()
		{
			::FreeImage_DeInitialise();
		}

		void OpenGL_Base::Init()
		{
		}

		void OpenGL_Base::Begin()
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			//glAlphaFunc( GL_GREATER, 1.0f );
		}

		void OpenGL_Base::End()
		{
			Flush();
		}

		void OpenGL_Base::Flush()
		{
			if ( m_iVertNum == 0 ) { return; }

			glVertexPointer( 3, GL_FLOAT,  sizeof( Vertex ), ( void* ) &m_Vertices[0].x );
			glEnableClientState( GL_VERTEX_ARRAY );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), ( void* ) &m_Vertices[0].r );
			glEnableClientState( GL_COLOR_ARRAY );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), ( void* ) &m_Vertices[0].u );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glDrawArrays( GL_TRIANGLES, 0, ( GLsizei ) m_iVertNum );
			m_iVertNum = 0;
			glFlush();
		}

		void OpenGL_Base::AddVert( int x, int y, float u, float v )
		{
			if ( m_iVertNum >= MaxVerts - 1 )
			{
				Flush();
			}

			m_Vertices[ m_iVertNum ].x = ( float ) x;
			m_Vertices[ m_iVertNum ].y = ( float ) y;
			m_Vertices[ m_iVertNum ].u = u;
			m_Vertices[ m_iVertNum ].v = v;
			m_Vertices[ m_iVertNum ].r = m_Color.r;
			m_Vertices[ m_iVertNum ].g = m_Color.g;
			m_Vertices[ m_iVertNum ].b = m_Color.b;
			m_Vertices[ m_iVertNum ].a = m_Color.a;
			m_iVertNum++;
		}

		void OpenGL_Base::DrawFilledRect( Gwen::Rect rect )
		{
			GLboolean texturesOn;
			glGetBooleanv( GL_TEXTURE_2D, &texturesOn );

			if ( texturesOn )
			{
				Flush();
				glDisable( GL_TEXTURE_2D );
			}

            //SetDrawColor(Gwen::Color(255, 0, 0, 255));

			Translate( rect );
			AddVert( rect.x, rect.y );
			AddVert( rect.x + rect.w, rect.y );
			AddVert( rect.x, rect.y + rect.h );
			AddVert( rect.x + rect.w, rect.y );
			AddVert( rect.x + rect.w, rect.y + rect.h );
			AddVert( rect.x, rect.y + rect.h );
		}

		void OpenGL_Base::SetDrawColor( Gwen::Color color )
		{
			glColor4ubv( ( GLubyte* ) &color );
			m_Color = color;
		}

		void OpenGL_Base::StartClip()
		{
            //return;
			Flush();
			Gwen::Rect rect = ClipRegion();
			// OpenGL's coords are from the bottom left
			// so we need to translate them here.
			{
				GLint view[4];
				glGetIntegerv( GL_VIEWPORT, &view[0] );
				rect.y = view[3] - ( rect.y + rect.h );
			}
			glScissor( rect.x * Scale(), rect.y * Scale(), rect.w * Scale(), rect.h * Scale() );
			glEnable( GL_SCISSOR_TEST );
		}

		void OpenGL_Base::EndClip()
		{
            //return;
			Flush();
			glDisable( GL_SCISSOR_TEST );
		}

		void OpenGL_Base::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
			GLuint* tex = ( GLuint* ) pTexture->data;
            //tex = &last_tex;

			// Missing image, not loaded properly?
			if ( !tex )
			{
				return DrawMissingImage( rect );
			}

			Translate( rect );
			GLuint boundtex;
			GLboolean texturesOn;
			glGetBooleanv( GL_TEXTURE_2D, &texturesOn );
			glGetIntegerv( GL_TEXTURE_BINDING_2D, ( GLint* ) &boundtex );

			if ( !texturesOn || *tex != boundtex )
			{
				Flush();
				glBindTexture( GL_TEXTURE_2D, *tex );
				glEnable( GL_TEXTURE_2D );
			}

			AddVert( rect.x, rect.y,			u1, v1 );
			AddVert( rect.x + rect.w, rect.y,		u2, v1 );
			AddVert( rect.x, rect.y + rect.h,	u1, v2 );
			AddVert( rect.x + rect.w, rect.y,		u2, v1 );
			AddVert( rect.x + rect.w, rect.y + rect.h, u2, v2 );
			AddVert( rect.x, rect.y + rect.h, u1, v2 );
		}

		void OpenGL_Base::LoadTexture( Gwen::Texture* pTexture )
		{
#ifdef _WIN32
			const wchar_t* wFileName = pTexture->name.GetUnicode().c_str();
			FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileTypeU( wFileName );

			if ( imageFormat == FIF_UNKNOWN )
			{ imageFormat = FreeImage_GetFIFFromFilenameU( wFileName ); }
#else
			const char* fileName = pTexture->name.m_String.c_str();
			FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType( fileName );

			if ( imageFormat == FIF_UNKNOWN )
			{ imageFormat = FreeImage_GetFIFFromFilename( fileName ); }
#endif

			// Image failed to load..
			if ( imageFormat == FIF_UNKNOWN )
			{
				pTexture->failed = true;
				return;
			}

			// Try to load the image..
#ifdef _WIN32
			FIBITMAP* bits = FreeImage_LoadU( imageFormat, wFileName );
#else
			FIBITMAP* bits = FreeImage_Load( imageFormat, fileName );
#endif

			if ( !bits )
			{
				pTexture->failed = true;
				return;
			}

			// Convert to 32bit
			FIBITMAP* bits32 = FreeImage_ConvertTo32Bits( bits );
			FreeImage_Unload( bits );

			if ( !bits32 )
			{
				pTexture->failed = true;
				return;
			}

			// Flip
			::FreeImage_FlipVertical( bits32 );
			// Create a little texture pointer..
			GLuint* pglTexture = new GLuint;
			// Sort out our GWEN texture
			pTexture->data = pglTexture;
			pTexture->width = FreeImage_GetWidth( bits32 );
			pTexture->height = FreeImage_GetHeight( bits32 );
			// Create the opengl texture
			glGenTextures( 1, pglTexture );
			glBindTexture( GL_TEXTURE_2D, *pglTexture );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#ifdef FREEIMAGE_BIGENDIAN
			GLenum format = GL_RGBA;
#else
			GLenum format = GL_BGRA;
#endif
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->width, pTexture->height, 0, format, GL_UNSIGNED_BYTE, ( const GLvoid* ) FreeImage_GetBits( bits32 ) );
			FreeImage_Unload( bits32 );
		}

		void OpenGL_Base::FreeTexture( Gwen::Texture* pTexture )
		{
			GLuint* tex = ( GLuint* ) pTexture->data;

			if ( !tex ) { return; }

			glDeleteTextures( 1, tex );
			delete tex;
			pTexture->data = NULL;
		}

		Gwen::Color OpenGL_Base::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default )
		{
            //return col_default;
			GLuint* tex = ( GLuint* ) pTexture->data;

			if ( !tex ) { return col_default; }

			unsigned int iPixelSize = sizeof( unsigned char ) * 4;
			glBindTexture( GL_TEXTURE_2D, *tex );
			unsigned char* data = ( unsigned char* ) malloc( iPixelSize * pTexture->width * pTexture->height );
			glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
			unsigned int iOffset = ( y * pTexture->width + x ) * 4;
			Gwen::Color c;
			c.r = data[0 + iOffset];
			c.g = data[1 + iOffset];
			c.b = data[2 + iOffset];
			c.a = data[3 + iOffset];
			//
			// Retrieving the entire texture for a single pixel read
			// is kind of a waste - maybe cache this pointer in the texture
			// data and then release later on? It's never called during runtime
			// - only during initialization.
			//
			free( data );
			return c;
		}

		bool OpenGL_Base::InitializeContext( Gwen::WindowProvider* pWindow )
		{
#ifdef _WIN32
			HWND pHwnd = ( HWND ) pWindow->GetWindow();

			if ( !pHwnd ) { return false; }

			HDC hDC = GetDC( pHwnd );
			//
			// Set the pixel format
			//
			PIXELFORMATDESCRIPTOR pfd;
			memset( &pfd, 0, sizeof( pfd ) );
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 24;
			pfd.cDepthBits = 32;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int iFormat = ChoosePixelFormat( hDC, &pfd );
			SetPixelFormat( hDC, iFormat, &pfd );
			HGLRC hRC;
			hRC = wglCreateContext( hDC );
			wglMakeCurrent( hDC, hRC );
			RECT r;

			if ( GetClientRect( pHwnd, &r ) )
			{
				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();
				glOrtho( r.left, r.right, r.bottom, r.top, -1.0, 1.0 );
				glMatrixMode( GL_MODELVIEW );
				glViewport( 0, 0, r.right - r.left, r.bottom - r.top );
			}

			m_pContext = ( void* ) hRC;
			return true;
#else
			Window win = (Window)pWindow->GetWindow();
			// Get the default screen's GLX extension list
			const char *glxExts = glXQueryExtensionsString( x11_display,
                                                  DefaultScreen( x11_display ) );

			// NOTE: It is not necessary to create or make current to a context before
			// calling glXGetProcAddressARB
			glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
			glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
			glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

			GLXContext ctx = 0;

			// Install an X error handler so the application won't exit if GL 3.0
			// context allocation fails.
			//
			// Note this error handler is global.  All display connections in all threads
			// of a process use the same error handler, so be sure to guard against other
			// threads issuing X commands while this code is running.
			ctxErrorOccurred = false;
			int (*oldHandler)(Display*, XErrorEvent*) =
			XSetErrorHandler(&ctxErrorHandler);

			// Check for the GLX_ARB_create_context extension string and the function.
			// If either is not present, use GLX 1.3 context creation method.
			if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
				!glXCreateContextAttribsARB )
			{
				printf( "glXCreateContextAttribsARB() not found"
					" ... using old-style GLX context\n" );
				ctx = glXCreateNewContext( x11_display, global_bestFbc, GLX_RGBA_TYPE, 0, True );
			}
			// If it does, try to get a GL 3.0 context!
			else
			{
				int context_attribs[] =
				{
					GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
					GLX_CONTEXT_MINOR_VERSION_ARB, 0,// change back to 0
					//GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					None
				};

				//printf( "Creating context\n" );
				ctx = glXCreateContextAttribsARB( x11_display, global_bestFbc, 0,
                                      True, context_attribs );

				// Sync to ensure any errors generated are processed.
				XSync( x11_display, False );
				if ( !ctxErrorOccurred && ctx )
				{
					//  printf( "Created GL 3.0 context\n" );
				}
				else
				{
					// Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
					// When a context version below 3.0 is requested, implementations will
					// return the newest context version compatible with OpenGL versions less
					// than version 3.0.
					// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
					context_attribs[1] = 1;
					// GLX_CONTEXT_MINOR_VERSION_ARB = 0
					context_attribs[3] = 0;

					ctxErrorOccurred = false;

					printf( "Failed to create GL 3.0 context"
						" ... using old-style GLX context\n" );
					ctx = glXCreateContextAttribsARB( x11_display, global_bestFbc, 0, 
                                        True, context_attribs );
				}
			}

			// Sync to ensure any errors generated are processed.
			XSync( x11_display, False );

			// Restore the original error handler
			XSetErrorHandler( oldHandler );

			if ( ctxErrorOccurred || !ctx )
			{
				printf( "Failed to create an OpenGL context\n" );
				exit(1);
			}

			// Verifying that context is a direct context
			if ( ! glXIsDirect ( x11_display, ctx ) )
			{
				printf( "Indirect GLX rendering context obtained\n" );
			}
			else
			{
				//printf( "Direct GLX rendering context obtained\n" );
			}

			//printf( "Making context current\n" );
			glXMakeCurrent( x11_display, win, ctx );

			m_pContext = (GLXContext)ctx;
            return true;
#endif
			return false;
		}

		bool OpenGL_Base::ShutdownContext( Gwen::WindowProvider* pWindow )
		{
#ifdef _WIN32
			wglDeleteContext( ( HGLRC ) m_pContext );
			return true;
#else
			glXMakeCurrent( x11_display, 0, 0 );
			glXDestroyContext( x11_display, (GLXContext)m_pContext );
            return true;
#endif
			return false;
		}

		bool OpenGL_Base::PresentContext( Gwen::WindowProvider* pWindow )
		{
#ifdef _WIN32
			HWND pHwnd = ( HWND ) pWindow->GetWindow();

			if ( !pHwnd ) { return false; }

			HDC hDC = GetDC( pHwnd );
			SwapBuffers( hDC );
			return true;
#else
			Window window = ( Window ) pWindow->GetWindow();

			if ( !window ) { return false; }

            glXSwapBuffers ( x11_display, window );
            return true;
#endif
			return false;
		}

		bool OpenGL_Base::ResizedContext( Gwen::WindowProvider* pWindow, int w, int h )
		{
#ifdef _WIN32
			RECT r;

			if ( GetClientRect( ( HWND ) pWindow->GetWindow(), &r ) )
			{
				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();
				glOrtho( r.left, r.right, r.bottom, r.top, -1.0, 1.0 );
				glMatrixMode( GL_MODELVIEW );
				glViewport( 0, 0, r.right - r.left, r.bottom - r.top );
			}

			return true;
#else
            struct R {  int left, right, bottom, top; };
            R r;
            Window root;
            unsigned int width, height, border_width, depth;
            XGetGeometry(x11_display, (Window)pWindow->GetWindow(), &root, &r.left, &r.top, &width, &height, &border_width, &depth);
            r.right = r.left + width;
            r.bottom = r.top + height;

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			glOrtho( 0, r.right-r.left, r.bottom-r.top, 0, -1.0, 1.0 );
			glMatrixMode( GL_MODELVIEW );
			glViewport(0, 0, width, height);
            return true;
#endif
			return false;
		}

		bool OpenGL_Base::BeginContext( Gwen::WindowProvider* pWindow )
		{
			glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			return true;
		}

		bool OpenGL_Base::EndContext( Gwen::WindowProvider* pWindow )
		{
			return true;
		}

	}
}
