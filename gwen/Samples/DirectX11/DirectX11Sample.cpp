#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/DirectX11.h"

uint32_t width = 1004;
uint32_t height = 650;

HINSTANCE hInstance;
HWND hWnd;

IDXGISwapChain* swapChain = NULL;
ID3D11Device* device = NULL;
ID3D11DeviceContext* dc = NULL;
ID3D11RenderTargetView* renderTarget = NULL;

HWND CreateGameWindow( void )
{
    hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = DefWindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    wc.hIconSm       = wc.hIcon;
    wc.hCursor       = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"GWENWindow";
    wc.cbSize        = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        wc.lpszClassName,
        L"GWEN - DirectX 11 Sample",
        ( WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ) & ~( WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME ),
        -1,
        -1,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        NULL);

    RECT clientRect;
    GetClientRect(hWnd,&clientRect);
    width = uint32_t(clientRect.right - clientRect.left);
    height = uint32_t(clientRect.bottom - clientRect.top);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    return hWnd;
}


void CreateD3DDevice()
{
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC)); 

    scd.BufferCount = 1; 
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    scd.BufferDesc.Width = width; 
    scd.BufferDesc.Height = height; 
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
    scd.OutputWindow = hWnd; 
    scd.SampleDesc.Count = 1; 
    scd.Windowed = TRUE; 
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; 

    D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
#ifndef NDEBUG
        D3D11_CREATE_DEVICE_DEBUG,
#else
        NULL,
#endif
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapChain,
        &device,
        NULL,
        &dc); 

    ID3D11Texture2D *pBackBuffer; 
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer); 

    device->CreateRenderTargetView(pBackBuffer, NULL, &renderTarget); 
    pBackBuffer->Release(); 

    dc->OMSetRenderTargets(1, &renderTarget, NULL); 

    D3D11_VIEWPORT viewport; 
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT)); 
    viewport.TopLeftX = 0; 
    viewport.TopLeftY = 0; 
    viewport.Width = (float)width; 
    viewport.Height = (float)height; 
    dc->RSSetViewports(1, &viewport);
}

//
// Program starts here
//
int main(void)
{
    // Create a window and attach directx to it
    hWnd = CreateGameWindow();
    CreateD3DDevice();

    RECT FrameBounds;
    GetClientRect( hWnd, &FrameBounds );

    // Create a GWEN DirectX renderer
    Gwen::Renderer::DirectX11* pRenderer = new Gwen::Renderer::DirectX11(device, dc, (float)width, (float)height);

    // Create a GWEN skin
    Gwen::Skin::TexturedBase* pSkin = new Gwen::Skin::TexturedBase( pRenderer );
    pSkin->Init( "DefaultSkin.png" );

    // Create a Canvas (it's root, on which all other GWEN panels are created)
    Gwen::Controls::Canvas* pCanvas = new Gwen::Controls::Canvas( pSkin );
    pCanvas->SetSize( FrameBounds.right, FrameBounds.bottom );
    pCanvas->SetDrawBackground( true );
    pCanvas->SetBackgroundColor( Gwen::Color( 150, 170, 170, 255 ) );

    // Create our unittest control (which is a Window with controls in it)
    UnitTest* pUnit = new UnitTest( pCanvas );
    pUnit->SetPos( 10, 10 );

    // Create a Windows Control helper
    // (Processes Windows MSG's and fires input at GWEN)
    Gwen::Input::Windows GwenInput;
    GwenInput.Initialize( pCanvas );

    // Begin the main game loop
    MSG msg;

    while ( true )
    {
        // Skip out if the window is closed
        if ( !IsWindowVisible( hWnd ) ) break;

        // If we have a message from windows..
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // .. give it to the input handler to process
            GwenInput.ProcessMessage( msg );

            // if it's QUIT then quit..
            if ( msg.message == WM_QUIT ) break;

            // Handle the regular window stuff..
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            float clearColor[4] = {0.0f,0.0f,0.0f,0.0f};
            dc->ClearRenderTargetView(renderTarget,clearColor);
            pCanvas->RenderCanvas();
            swapChain->Present(0,0);
        }
    }

    delete pCanvas;
    delete pSkin;
    delete pRenderer;
}