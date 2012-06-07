#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/Direct2D.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

HWND					g_pHWND = NULL;
ID2D1Factory*			g_pD2DFactory = NULL;
IDWriteFactory*			g_pDWriteFactory = NULL;
IWICImagingFactory*		g_pWICFactory = NULL;
ID2D1HwndRenderTarget*	g_pRT = NULL; // this is device-specific

Gwen::Renderer::Direct2D* g_pRenderer = NULL;

//
// Windows bullshit to create a Window to render to.
//
HWND CreateGameWindow( void )
{
	WNDCLASS	wc;
	ZeroMemory( &wc, sizeof( wc ) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= DefWindowProc;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= L"GWENWindow";
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );

	RegisterClass( &wc );

	HWND hWindow = CreateWindowEx( (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) , wc.lpszClassName, L"GWEN - Direct 2D Sample", (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME), -1, -1, 1004, 650, NULL, NULL, GetModuleHandle(NULL), NULL );

	ShowWindow( hWindow, SW_SHOW );
	SetForegroundWindow( hWindow );
	SetFocus( hWindow );

	return hWindow;
}

HRESULT createDeviceResources();
void discardDeviceResources();
void runSample();

//
// Program starts here
//
int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&g_pD2DFactory
		);

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&g_pDWriteFactory)
	);
	
	hr = CoInitialize(NULL);

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void **>(&g_pWICFactory)
	);

	g_pHWND = CreateGameWindow();

	createDeviceResources();

	//
	// Create a GWEN Direct2D renderer
	//
	g_pRenderer = new Gwen::Renderer::Direct2D( g_pRT, g_pDWriteFactory, g_pWICFactory );

	runSample();

	delete g_pRenderer;
	g_pRenderer = NULL;

	if( g_pRT != NULL )
	{
		g_pRT->Release();
		g_pRT = NULL;
	}
}

HRESULT createDeviceResources()
{
    HRESULT hr = S_OK;

    if ( !g_pRT )
    {
        RECT rc;
        GetClientRect(g_pHWND, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = g_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(g_pHWND, size),
            &g_pRT
            );

		if ( SUCCEEDED( hr ) && g_pRenderer != NULL )
		{
			g_pRT->SetTextAntialiasMode( D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE );

			g_pRenderer->DeviceAcquired( g_pRT );
		}
	}
	
	return hr;
}

void discardDeviceResources()
{
	if ( g_pRT != NULL )
	{
		g_pRT->Release();
		g_pRT = NULL;
	}
}

void runSample()
{
	RECT FrameBounds;
	GetClientRect(g_pHWND, &FrameBounds);

	//
	// Create a GWEN skin
	//
	Gwen::Skin::TexturedBase skin( g_pRenderer );
	skin.Init( "DefaultSkin.png" );

	//
	// Create a Canvas (it's root, on which all other GWEN panels are created)
	//
	Gwen::Controls::Canvas* pCanvas = new Gwen::Controls::Canvas( &skin );
	pCanvas->SetSize( FrameBounds.right, FrameBounds.bottom );
	pCanvas->SetDrawBackground( true );
	pCanvas->SetBackgroundColor( Gwen::Color( 150, 170, 170, 255 ) );

	//
	// Create our unittest control (which is a Window with controls in it)
	//
	UnitTest* pUnit = new UnitTest( pCanvas );
	pUnit->SetPos( 10, 10 );

	//
	// Create a Windows Control helper 
	// (Processes Windows MSG's and fires input at GWEN)
	//
	Gwen::Input::Windows GwenInput;
	GwenInput.Initialize( pCanvas );

	//
	// Begin the main game loop
	//
	MSG msg;

	while( true )
	{
		// Skip out if the window is closed
		if ( !IsWindowVisible( g_pHWND ) )
			break;

		// If we have a message from windows..
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			// .. give it to the input handler to process
			GwenInput.ProcessMessage( msg );

			// if it's QUIT then quit..
			if ( msg.message == WM_QUIT )
				break;

			// Handle the regular window stuff..
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		{
			if ( SUCCEEDED( createDeviceResources() ) )
			{
				g_pRT->BeginDraw();
				g_pRT->SetTransform( D2D1::Matrix3x2F::Identity() );
				g_pRT->Clear( D2D1::ColorF( D2D1::ColorF::White ) );

				// This is how easy it is to render GWEN!
				pCanvas->RenderCanvas();

				HRESULT hr = g_pRT->EndDraw();

				if ( hr == D2DERR_RECREATE_TARGET )
				{
					discardDeviceResources();
					g_pRenderer->DeviceLost();
				}
			}
		}
	}

	delete pCanvas;
}
