/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifdef _WIN32

#ifndef _WIN32_WINNT
//#	define _WIN32_WINNT 0x0600
#else
#	if _WIN32_WINNT < 0x0600
#		error Unsupported platform
#	endif
#endif

#include "Gwen/Macros.h"
#include "Gwen/Platform.h"
#include "Gwen/Input/Windows.h"

#include <windows.h>
#include <ShlObj.h>
#include <Shobjidl.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shcore.lib")
using namespace Gwen;
using namespace Gwen::Platform;

static const size_t FileStringSize		= 256;
static const size_t FilterBufferSize	= 512;

static Gwen::Input::Windows GwenInput;

static LPCTSTR iCursorConversion[] =
{
	IDC_ARROW,
	IDC_IBEAM,
	IDC_SIZENS,
	IDC_SIZEWE,
	IDC_SIZENWSE,
	IDC_SIZENESW,
	IDC_SIZEALL,
	IDC_NO,
	IDC_WAIT,
	IDC_HAND
};

static bool loaded_cursors = false;
static HCURSOR cursors[10];
static HCURSOR current_cursor;
void Gwen::Platform::SetCursor(unsigned char iCursor)
{
	if (!loaded_cursors)
	{
		for (int i = 0; i < 10; i++)
		{
			cursors[i] = LoadCursor(NULL, iCursorConversion[i]);
		}
		loaded_cursors = true;
	}

	if (GetCursor() != cursors[iCursor])
	{
		::SetCursor(cursors[iCursor]);
		current_cursor = cursors[iCursor];
	}
}

void Gwen::Platform::GetCursorPos( Gwen::Point & po )
{
	POINT p;
	::GetCursorPos( &p );
	po.x = p.x;
	po.y = p.y;
}

void Gwen::Platform::GetDesktopSize( int & w, int & h )
{
	w = GetSystemMetrics( SM_CXFULLSCREEN );
	h = GetSystemMetrics( SM_CYFULLSCREEN );
}

Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
	if ( !OpenClipboard( NULL ) ) { return L""; }

	HANDLE hData = GetClipboardData( CF_UNICODETEXT );

	if ( hData == NULL )
	{
		CloseClipboard();
		return L"";
	}

	wchar_t* buffer = ( wchar_t* ) GlobalLock( hData );
	UnicodeString str = buffer;
	GlobalUnlock( hData );
	CloseClipboard();
	return str;
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString & str )
{
	if ( !OpenClipboard( NULL ) ) { return false; }

	EmptyClipboard();
	// Create a buffer to hold the string
	size_t iDataSize = ( str.length() + 1 ) * sizeof( wchar_t );
	HGLOBAL clipbuffer = GlobalAlloc( GMEM_DDESHARE, iDataSize );
	// Copy the string into the buffer
	wchar_t* buffer = ( wchar_t* ) GlobalLock( clipbuffer );
	wcscpy( buffer, str.c_str() );
	GlobalUnlock( clipbuffer );
	// Place it on the clipboard
	SetClipboardData( CF_UNICODETEXT, clipbuffer );
	CloseClipboard();
	return true;
}

double GetPerformanceFrequency()
{
	static double Frequency = 0.0f;

	if ( Frequency == 0.0f )
	{
		__int64 perfFreq;
		QueryPerformanceFrequency( ( LARGE_INTEGER* ) &perfFreq );
		Frequency = 1.0 / ( double ) perfFreq;
	}

	return Frequency;
}

float Gwen::Platform::GetTimeInSeconds()
{
	static float fCurrentTime = 0.0f;
	static __int64 iLastTime = 0;
	__int64 thistime;
	QueryPerformanceCounter( ( LARGE_INTEGER* ) &thistime );
	float fSecondsDifference = ( double )( thistime - iLastTime ) * GetPerformanceFrequency();

	if ( fSecondsDifference > 0.1f ) { fSecondsDifference = 0.1f; }

	fCurrentTime += fSecondsDifference;
	iLastTime = thistime;
	return fCurrentTime;
}



bool Gwen::Platform::FileOpen( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	char Filestring[FileStringSize];
	String returnstring;

	char FilterBuffer[FilterBufferSize];
	{
		memset( FilterBuffer, 0, sizeof( FilterBuffer ) );
		memcpy( FilterBuffer, Extension.c_str(), Gwen::Min( Extension.length(), sizeof( FilterBuffer ) ) );

		for ( int i = 0; i < FilterBufferSize; i++ )
		{
			if ( FilterBuffer[i] == '|' )
			{ FilterBuffer[i] = 0; }
		}
	}
	OPENFILENAMEA opf;
	opf.hwndOwner = 0;
	opf.lpstrFilter = FilterBuffer;
	opf.lpstrCustomFilter = 0;
	opf.nMaxCustFilter = 0L;
	opf.nFilterIndex = 1L;
	opf.lpstrFile = Filestring;
	opf.lpstrFile[0] = '\0';
	opf.nMaxFile = FileStringSize;
	opf.lpstrFileTitle = 0;
	opf.nMaxFileTitle = 50;
	opf.lpstrInitialDir = StartPath.c_str();
	opf.lpstrTitle = Name.c_str();
	opf.nFileOffset = 0;
	opf.nFileExtension = 0;
	opf.lpstrDefExt = "*.*";
	opf.lpfnHook = NULL;
	opf.lCustData = 0;
	opf.Flags = ( OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR ) & ~OFN_ALLOWMULTISELECT;
	opf.lStructSize = sizeof( OPENFILENAME );

	if ( GetOpenFileNameA( &opf ) )
	{
		if ( pHandler && fnCallback )
		{
			Gwen::Event::Information info;
			info.Control		= NULL;
			info.ControlCaller	= NULL;
			info.String			= opf.lpstrFile;
			( pHandler->*fnCallback )( info );
		}
	}

	return true;
}

bool Gwen::Platform::FolderOpen( const String & Name, const String & StartPath, Gwen::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback )
{
	IFileDialog* pfd = NULL;
	bool bSuccess = false;

#ifndef _MSC_VER
    // The line below is from the Qt Toolkit "src/plugins/platforms/windows/qwindowsdialoghelpers.cpp"
    static const CLSID CLSID_FileOpenDialog = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};
#endif

	if ( CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pfd ) ) != S_OK )
	{ return bSuccess; }

	DWORD dwOptions;

	if ( pfd->GetOptions( &dwOptions ) != S_OK )
	{
		pfd->Release();
		return bSuccess;
	}

	pfd->SetOptions( dwOptions | FOS_PICKFOLDERS );

	//
	// TODO: SetDefaultFolder -> StartPath
	//

	if ( pfd->Show( NULL ) == S_OK )
	{
		IShellItem* psi;

		if ( pfd->GetResult( &psi ) == S_OK )
		{
			WCHAR* strOut = NULL;

			if ( psi->GetDisplayName( SIGDN_DESKTOPABSOLUTEPARSING, &strOut ) != S_OK )
			{
				return bSuccess;
			}

			//
			// GWEN callback - call it.
			//
			if ( pHandler && fnCallback )
			{
				Gwen::Event::Information info;
				info.Control		= NULL;
				info.ControlCaller	= NULL;
				info.String			= Gwen::Utility::UnicodeToString( strOut );
				( pHandler->*fnCallback )( info );
			}

			CoTaskMemFree( strOut );
			psi->Release();
			bSuccess = true;
		}
	}

	pfd->Release();
	return bSuccess;
}

bool Gwen::Platform::FileSave( const String & Name, const String & StartPath, const String & Extension, Gwen::Event::Handler* pHandler, Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	char Filestring[FileStringSize];
	String returnstring;

	char FilterBuffer[FilterBufferSize];
	{
		memset( FilterBuffer, 0, sizeof( FilterBuffer ) );
		memcpy( FilterBuffer, Extension.c_str(), Gwen::Min( Extension.size(), sizeof( FilterBuffer ) ) );

		for ( int i = 0; i < FilterBufferSize; i++ )
		{
			if ( FilterBuffer[i] == '|' )
			{ FilterBuffer[i] = 0; }
		}
	}
	OPENFILENAMEA opf;
	opf.hwndOwner = 0;
	opf.lpstrFilter = FilterBuffer;
	opf.lpstrCustomFilter = 0;
	opf.nMaxCustFilter = 0L;
	opf.nFilterIndex = 1L;
	opf.lpstrFile = Filestring;
	opf.lpstrFile[0] = '\0';
	opf.nMaxFile = FileStringSize;
	opf.lpstrFileTitle = 0;
	opf.nMaxFileTitle = 50;
	opf.lpstrInitialDir = StartPath.c_str();
	opf.lpstrTitle = Name.c_str();
	opf.nFileOffset = 0;
	opf.nFileExtension = 0;
	opf.lpstrDefExt = "*.*";
	opf.lpfnHook = NULL;
	opf.lCustData = 0;
	opf.Flags = ( OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR ) & ~OFN_ALLOWMULTISELECT;
	opf.lStructSize = sizeof( OPENFILENAME );

	if ( GetSaveFileNameA( &opf ) )
	{
		if ( pHandler && fnCallback )
		{
			Gwen::Event::Information info;
			info.Control		= NULL;
			info.ControlCaller	= NULL;
			info.String			= opf.lpstrFile;
			( pHandler->*fnCallback )( info );
		}
	}

	return true;
}

static std::map<HWND, float> window_dpis;
static std::map<HWND, std::pair<int, int>> window_min_bounds;
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//case WM_NCLBUTTONDOWN:
	case WM_SYSCOMMAND:
	{
		if (message == WM_SYSCOMMAND && wParam != SC_CLOSE)
		{
			return false;
		}

		//return m_Canvas->InputQuit();
	}
	case WM_GETMINMAXINFO:
	{
		auto f = window_min_bounds.find(hwnd);
		if (f != window_min_bounds.end())
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = f->second.first;
			lpMMI->ptMinTrackSize.y = f->second.second;
		}
		break;
	}
	case WM_SETCURSOR:
	{
		// Sets the cursor if mouse input wasnt captured
		if (IsIconic(hwnd) && current_cursor)
		{
			SetCursor(current_cursor);
		}
		break;
	}
	case 0x02E0://WM_DPICHANGED:
	{   //todo handle x and y dpi
		float dpi = HIWORD(wParam);
		auto rect = *reinterpret_cast<RECT *>(lParam);
		SetWindowPos(hwnd,
			0, // No relative window
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOACTIVATE | SWP_NOZORDER);

		HRGN rgn = CreateRoundRectRgn(0, 0, rect.right - rect.left + 1, rect.bottom - rect.top + 1, 4, 4);
		SetWindowRgn(hwnd, rgn, false);

		window_dpis[hwnd] = dpi;
		return 0;
	}
	break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

#include <ShellScalingApi.h>

void* Gwen::Platform::CreatePlatformWindow(int x, int y, int w, int h, const Gwen::String & strWindowTitle, Gwen::Renderer::Base* renderer, bool is_menu)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	WNDCLASSW	wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_OWNDC | CS_DROPSHADOW;
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"GWEN_Window_Class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassW(&wc);

	// Get a guestimate DPI
	POINT p;
	p.x = x;
	p.y = y;
	auto monitor = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
	unsigned int dx, dy;
	GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dx, &dy);
	auto dpi = Gwen::PointF((float)dx/96.0f, (float)dy / 96.0f);
	
	HWND hWindow = CreateWindowExW( WS_EX_APPWINDOW | WS_EX_ACCEPTFILES, wc.lpszClassName, L"Testing"/*strWindowTitle.c_str()*/, WS_POPUP | WS_VISIBLE, x, y, w*dpi.x, h*dpi.y, NULL, NULL, GetModuleHandle( NULL ), NULL );
	ShowWindow( hWindow, SW_SHOW );
	SetForegroundWindow( hWindow );
	SetFocus( hWindow );

	SetWindowTextA(hWindow, strWindowTitle.c_str());
	HRGN rgn = CreateRoundRectRgn( 0, 0, w*dpi.x + 1, h*dpi.y + 1, 4, 4 );
	SetWindowRgn( hWindow, rgn, false );

	window_dpis[hWindow] = dy;
	return ( void* ) hWindow;
}

void Gwen::Platform::SetWindowTitle(void* pPtr, const Gwen::String & strWindowTitle)
{
	SetWindowTextA((HWND)pPtr, strWindowTitle.c_str());
}

void Gwen::Platform::DestroyPlatformWindow( void* pPtr )
{
	DestroyWindow( ( HWND ) pPtr );
	CoUninitialize();
	window_min_bounds.erase((HWND)pPtr);
	window_dpis.erase((HWND)pPtr);
}

int i = 0;
void Gwen::Platform::MessagePump( void* pWindow, Gwen::Controls::WindowCanvas* ptarget )
{
	MSG msg;

	while ( PeekMessage( &msg, ( HWND ) pWindow, 0, 0, PM_REMOVE ) )
	{
		if ( GwenInput.ProcessMessage( msg ) )
		{ continue; }

		if ( msg.message == WM_PAINT )
		{
			ptarget->Redraw();
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	if (ptarget->GetDPI() != window_dpis[(HWND)pWindow])
	{
		ptarget->SetDPI(window_dpis[(HWND)pWindow]);
		ptarget->SetScale(window_dpis[(HWND)pWindow] / 96.0);
		RECT rect;
		GetWindowRect((HWND)pWindow, &rect);
		ptarget->GetSkin()->GetRender()->ResizedContext( ptarget, rect.right - rect.left, rect.bottom - rect.top );
	}

	// If the active window has changed then force a redraw of our canvas
	// since we might paint ourselves a different colour if we're inactive etc
	{
		static HWND g_LastFocus = NULL;

		if ( GetActiveWindow() != g_LastFocus )
		{
			g_LastFocus = GetActiveWindow();
			ptarget->Redraw();
		}
	}
}

void Gwen::Platform::SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h )
{
	SetWindowPos( ( HWND ) pPtr, HWND_NOTOPMOST, x, y, w, h, SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOSENDCHANGING );
	// Curve the corners
	{
		HRGN rgn = CreateRoundRectRgn( 0, 0, w + 1, h + 1, 4, 4 );
		SetWindowRgn( ( HWND ) pPtr, rgn, false );
	}
}

void Gwen::Platform::SetWindowMaximized( void* pPtr, bool bMax, Gwen::Point & pNewPos, Gwen::Point & pNewSize )
{
	if ( bMax )
	{
		ShowWindow( ( HWND ) pPtr, SW_SHOWMAXIMIZED );
		RECT rect;
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );  // size excluding task bar
		SetWindowPos( ( HWND ) pPtr, HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOSENDCHANGING );
		// Remove the corner curves
		{
			SetWindowRgn( ( HWND ) pPtr, NULL, false );
		}
	}
	else
	{
		ShowWindow( ( HWND ) pPtr, SW_RESTORE );
		// Curve the corners
		{
			RECT r;
			GetWindowRect( ( HWND ) pPtr, &r );
			HRGN rgn = CreateRoundRectRgn( 0, 0, ( r.right - r.left ) + 1, ( r.bottom - r.top ) + 1, 4, 4 );
			SetWindowRgn( ( HWND ) pPtr, rgn, false );
		}
	}

	RECT r;
	GetWindowRect( ( HWND ) pPtr, &r );
	pNewSize.x = r.right - r.left;
	pNewSize.y = r.bottom - r.top ;
	pNewPos.x = r.left;
	pNewPos.y = r.top;
}

void Gwen::Platform::SetWindowMinimized( void* pPtr, bool bMinimized )
{
	if ( bMinimized )
	{
		ShowWindow( ( HWND ) pPtr, SW_SHOWMINIMIZED );
	}
	else
	{
		ShowWindow( ( HWND ) pPtr, SW_RESTORE );
	}
}

bool Gwen::Platform::HasFocusPlatformWindow( void* pPtr )
{
	return GetActiveWindow() == ( HWND ) pPtr;
}

void Gwen::Platform::Sleep( unsigned int iMS )
{
	::Sleep( iMS );
}

bool Gwen::Platform::WindowHasTitleBar()
{
	return false;
}

void Gwen::Platform::SetWindowMinimumSize( void* pPtr, int min_width, int min_height)
{
	window_min_bounds[(HWND)pPtr] = {min_width, min_height};
}

bool Gwen::Platform::IsWindowMaximized( void* pPtr)
{
	WINDOWPLACEMENT pl;
	GetWindowPlacement((HWND)pPtr, &pl);
	return pl.showCmd == SW_SHOWMAXIMIZED;
}

void Gwen::Platform::WaitForEvent( int delay_ms )
{
	// todo, I'm not sure that this works correctly
	MSG msg;
	if (!PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
	{
		if (delay_ms > 0)
		{
			MsgWaitForMultipleObjects(0, NULL, FALSE, delay_ms, QS_ALLINPUT);
		}
		else
		{
			WaitMessage();
		}
	}
}

void Gwen::Platform::InterruptWait()
{

}

#endif // WIN32
