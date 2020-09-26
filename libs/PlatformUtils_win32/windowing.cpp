#include "stdafx.h"
#include "windowing.h"

#include "core_platform/inc/windows_inc.h"


namespace RF { namespace platform { namespace windowing {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API shim::HWND CreateNewWindow( int width, int height, shim::WNDPROC WndProc )
{
	// Paradoxically, we must tell Windows we want to do all the new DPI
	//  scaling stuff, and that will make it NOT do any DPI scaling...
	win32::SetProcessDPIAware();

	// IDC_ARROW
	win32::LPWSTR const idc_arrow = reinterpret_cast<win32::LPWSTR>( static_cast<win32::ULONG_PTR>( static_cast<win32::WORD>( 32512 ) ) );

	win32::WNDCLASSW wc = {};
	wc.style = CS_OWNDC; // Individual instances of this window will not share device contexts.
	wc.lpfnWndProc = reinterpret_cast<win32::WNDPROC>( WndProc ); // The function that will be called for events.
	wc.cbClsExtra = 0; // The class has no extra memory.
	wc.cbWndExtra = 0; // The window has no extra memory.
	//wc.hInstance = hInstance; // The application that is managing the window.
	//wc.hIcon = LoadIcon( hInstance, 0 ); // We want to use resource 0 in this executable for our icon.
	wc.hCursor = win32::LoadCursorW( nullptr, idc_arrow ); // We want to use the standard arrow cursor.
	wc.hbrBackground = static_cast<win32::HBRUSH>( win32::GetStockObject( BLACK_BRUSH ) ); // The default background color.
	wc.lpszMenuName = nullptr; // The class has no menu.
	wc.lpszClassName = L"SimpleGL"; // The name of this class.
	RegisterClassW( &wc ); // Register the attributes for the next call to CreateWindow.

	win32::DWORD const windowStyles = WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE | WS_SIZEBOX;

	// Padding, for stuff like SM_CXFRAME, SM_CYFRAME, SM_CYCAPTION, etc.
	win32::RECT windRect = {};
	windRect.right = width;
	windRect.bottom = height;
	win32::AdjustWindowRect( &windRect, windowStyles, false );
	int const windowWidth = windRect.right - windRect.left;
	int const windowHeight = windRect.bottom - windRect.top;

	win32::HWND hWnd;
	hWnd = win32::CreateWindowExW( 0,
		L"SimpleGL", // The window class to use.
		L"SimpleGl Window", // The name to appear on the window.
		windowStyles, // Window styles.
		CW_USEDEFAULT,
		CW_USEDEFAULT, // Initial window position.
		windowWidth, // Default window mWidth.
		windowHeight, // Default window mHeight.
		nullptr, // The window has no parent.
		nullptr, // The window has no menu.
		nullptr, //hInstance, // The application that is managing the window.
		nullptr ); // Pointer to a value passed through WM_CREATE.

	return hWnd;
}



PLATFORMUTILS_API int32_t ProcessAllMessages()
{
	int32_t retVal = 0;

	// Pump until empty
	int32_t messageResult;
	while( ( messageResult = ProcessSingleMessage() ) > 0 )
	{
		retVal++;
	}

	if( messageResult < 0 )
	{
		// Should quit
		return -retVal;
	}

	return retVal;
}



PLATFORMUTILS_API int32_t ProcessSingleMessage()
{
	bool shouldKeepRunning = true;
	bool processedMessage = false;

	// Check for any new messages
	win32::MSG msg;
	if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		processedMessage = true;

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

	if( shouldKeepRunning == false )
	{
		return -1;
	}

	if( processedMessage == false )
	{
		return 0;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
}}}
