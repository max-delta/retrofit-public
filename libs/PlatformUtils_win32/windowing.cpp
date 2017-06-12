#include "stdafx.h"
#include "windowing.h"

#include "core_platform/windows_inc.h"

namespace RF { namespace platform { namespace windowing {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API shim::HWND CreateNewWindow( int width, int height, shim::WNDPROC WndProc )
{
	// IDC_ARROW
	auto idc_arrow = ((win32::LPWSTR)((win32::ULONG_PTR)((win32::WORD)(32512))));

	win32::WNDCLASS wc;
	wc.style = CS_OWNDC; // Individual instances of this window will not share device contexts.
	wc.lpfnWndProc = (win32::WNDPROC)WndProc; // The function that will be called for events.
	wc.cbClsExtra = 0; // The class has no extra memory.
	wc.cbWndExtra = 0; // The window has no extra memory.
//	wc.hInstance = hInstance; // The application that is managing the window.
//	wc.hIcon = LoadIcon( hInstance, 0 ); // We want to use resource 0 in this executable for our icon.
	wc.hCursor = win32::LoadCursorW( nullptr, idc_arrow ); // We want to use the standard arrow cursor.
	wc.hbrBackground = (win32::HBRUSH)win32::GetStockObject( BLACK_BRUSH ); // The default background color.
	wc.lpszMenuName = nullptr; // The class has no menu.
	wc.lpszClassName = L"SimpleGL"; // The name of this class.
	RegisterClassW( &wc ); // Register the attributes for the next call to CreateWindow.

	win32::HWND hWnd;
	hWnd = win32::CreateWindowExW( 0,
		L"SimpleGL", // The window class to use.
		L"SimpleGl Window", // The name to appear on the window.
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE | WS_SIZEBOX, // Window styles.
		CW_USEDEFAULT, CW_USEDEFAULT, // Initial window position.
		width + win32::GetSystemMetrics( SM_CXFRAME ) * 2, // Default window width.
		height + win32::GetSystemMetrics( SM_CYFRAME ) * 2 + win32::GetSystemMetrics( SM_CYCAPTION ), // Default window height.
		nullptr, // The window has no parent.
		nullptr, // The window has no menu.
		nullptr,//hInstance, // The application that is managing the window.
		nullptr ); // Pointer to a value passed through WM_CREATE.

	return hWnd;
}



PLATFORMUTILS_API bool ProcessMessages()
{
	bool shouldKeepRunning = true;

	// Check for any new messages
	win32::MSG msg;
	if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_QUIT )
		{
			// User wants out
			shouldKeepRunning = false;
		}
		else
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return shouldKeepRunning;
}

///////////////////////////////////////////////////////////////////////////////
}}}
