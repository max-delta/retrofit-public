#include "stdafx.h"
#include "windowing.h"

#include "core_platform/inc/windows_inc.h"
#include "core_unicode/CharConvert.h"

#include "rftl/string"


namespace RF::platform::windowing {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static math::AABB4i32 AABBFromRect( win32::RECT const& rect )
{
	return math::AABB4i32( rect.left, rect.top, rect.right, rect.bottom );
}



static win32::DWORD ComputeWindowStyle( WindowStyle style )
{
	static constexpr win32::DWORD kAddTitleBar =
		WS_CAPTION | WS_SYSMENU;
	static constexpr win32::DWORD kAllowResizing =
		WS_SIZEBOX;

	static constexpr win32::DWORD kLegacyStyle =
		WS_VISIBLE | kAddTitleBar | kAllowResizing | WS_POPUP | WS_BORDER;
	static constexpr win32::DWORD kStandardStyle =
		WS_VISIBLE | kAddTitleBar | kAllowResizing | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	static constexpr win32::DWORD kBorderlessStyle =
		WS_VISIBLE | WS_POPUP | WS_MAXIMIZE;

	// NOTE: Even though it shouldn't be able to accept minimize or restore
	//  commands, it's still possible to force them (such as with Win+Down),
	//  which breaks the window in a way that can't be recovered, since the
	//  maximize IS correctly blocked, so we have to explicitly enable the
	//  maximize command so that a user can potentially recover their window
	//  without just restarting the whole application if they manage to get
	//  themselves into such a state
	// NOTE: The commands could theoretically be intercepted at the message
	//  pump before they can execute, but there's cases (like Win+D) where we
	//  probably still want to respect a forced window operation, even though
	//  it's against the intended flow
	static_assert( ( kBorderlessStyle & WS_SYSMENU ) == 0 );
	static_assert( ( kBorderlessStyle & WS_MINIMIZEBOX ) == 0 );
	static constexpr win32::DWORD kHACKRecoverBorderlessFullScreen = WS_MAXIMIZEBOX;

	switch( style )
	{
		case WindowStyle::Legacy:
			return kLegacyStyle;
		case WindowStyle::Standard:
			return kStandardStyle;
		case WindowStyle::Borderless:
			return kBorderlessStyle | kHACKRecoverBorderlessFullScreen;
		case WindowStyle::Invalid:
		default:
			RF_DBGFAIL();
			return kLegacyStyle;
	}
}



static math::Vector2i32 ComputeWindowPosition( win32::RECT const& shape, WindowStyle style )
{
	static constexpr auto centered = []( win32::RECT const& rect ) -> math::Vector2i32
	{
		math::AABB4i32 const window = AABBFromRect( rect );
		math::AABB4i32 const desktop = GetDesktopShape();
		int32_t const x = ( desktop.Width() - window.Width() ) / 2;
		int32_t const y = ( desktop.Height() - window.Height() ) / 2;
		RF_ASSERT( x >= 0 );
		RF_ASSERT( y >= 0 );
		return { x, y };
	};

	switch( style )
	{
		case WindowStyle::Legacy:
			// The default ends up in the upper left corner
			return { CW_USEDEFAULT, CW_USEDEFAULT };
		case WindowStyle::Standard:
			// The default ends up in a random location that is often partially
			//  off-screen, Windows can't be trusted for this
			return centered( shape );
		case WindowStyle::Borderless:
			// The default ends up in the upper left corner
			return { CW_USEDEFAULT, CW_USEDEFAULT };
		case WindowStyle::Invalid:
		default:
			RF_DBGFAIL();
			return { CW_USEDEFAULT, CW_USEDEFAULT };
	}
}

}
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API shim::HWND CreateNewWindow(
	WindowStyle windowStyle,
	rftl::string_view windowTitle,
	uint32_t width,
	uint32_t height,
	shim::WNDPROC wndProc )
{
	RF_ASSERT( windowStyle != WindowStyle::Invalid );
	RF_ASSERT( windowTitle.empty() == false );
	RF_ASSERT( width > 0 );
	RF_ASSERT( height > 0 );

	// Window title can't be unicode
	rftl::wstring ucs2WindowTitle;
	ucs2WindowTitle.reserve( windowTitle.size() );
	for( char const& ch : windowTitle )
	{
		if( unicode::IsValidAscii( ch ) )
		{
			ucs2WindowTitle.push_back( math::integer_cast<wchar_t>( ch ) );
		}
		else
		{
			RF_DBGFAIL_MSG( "Unsafe window title is not ASCII" );
			ucs2WindowTitle.push_back( L'?' );
		}
	}

	// Use default wndProc if none provided
	if( wndProc == nullptr )
	{
		wndProc = shim::DefWindowProcW;
	}

	// HWND pointer target is void in shim and stub struct in win32
	win32::WNDPROC adjustedWndProc = nullptr;
	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Wcast-function-type-strict" );
	adjustedWndProc = reinterpret_cast<win32::WNDPROC>( wndProc );
	RF_CLANG_POP();

	// Paradoxically, we must tell Windows we want to do all the new DPI
	//  scaling stuff, and that will make it NOT do any DPI scaling...
	win32::SetProcessDPIAware();

	// IDC_ARROW
	win32::LPWSTR const idc_arrow =
		reinterpret_cast<win32::LPWSTR>(
			static_cast<win32::ULONG_PTR>(
				static_cast<win32::WORD>( 32512u ) ) );

	// Create the window class
	static constexpr wchar_t kWindowClass[] = L"RetroFitPrimary";
	win32::WNDCLASSW wc = {};
	wc.style = CS_OWNDC; // Individual instances of this window will not share device contexts.
	wc.lpfnWndProc = adjustedWndProc; // The function that will be called for events.
	wc.cbClsExtra = 0; // The class has no extra memory.
	wc.cbWndExtra = 0; // The window has no extra memory.
	//wc.hInstance = hInstance; // The application that is managing the window.
	//wc.hIcon = LoadIcon( hInstance, 0 ); // We want to use resource 0 in this executable for our icon.
	wc.hCursor = win32::LoadCursorW( nullptr, idc_arrow ); // We want to use the standard arrow cursor.
	wc.hbrBackground = static_cast<win32::HBRUSH>( win32::GetStockObject( BLACK_BRUSH ) ); // The default background color.
	wc.lpszMenuName = nullptr; // The class has no menu.
	wc.lpszClassName = kWindowClass; // The name of this class.
	RegisterClassW( &wc ); // Register the attributes for the next call to CreateWindow.

	// Window style
	win32::DWORD const computedWindowStyle = details::ComputeWindowStyle( windowStyle );

	// Padding, for stuff like SM_CXFRAME, SM_CYFRAME, SM_CYCAPTION, etc.
	win32::RECT windRect = {};
	windRect.right = math::integer_cast<win32::LONG>( width );
	windRect.bottom = math::integer_cast<win32::LONG>( height );
	win32::AdjustWindowRect( &windRect, computedWindowStyle, false );
	win32::LONG const windowWidth = windRect.right - windRect.left;
	win32::LONG const windowHeight = windRect.bottom - windRect.top;

	// Extended style flags
	static constexpr win32::DWORD kExtendedStyle =
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	static_assert( kExtendedStyle == 0, "Expected this to be the default" );

	// Window position
	math::Vector2i32 const windowPos = details::ComputeWindowPosition( windRect, windowStyle );

	// Create the actual window
	win32::HWND const hWnd = win32::CreateWindowExW(
		kExtendedStyle,
		kWindowClass, // The window class to use.
		ucs2WindowTitle.data(), // The name to appear on the window.
		computedWindowStyle, // Window style.
		windowPos.x, // Initial window position.
		windowPos.y, // Initial window position.
		windowWidth, // Default window mWidth.
		windowHeight, // Default window mHeight.
		nullptr, // The window has no parent.
		nullptr, // The window has no menu.
		nullptr, //hInstance, // The application that is managing the window.
		nullptr ); // Pointer to a value passed through WM_CREATE.

	return hWnd;
}



PLATFORMUTILS_API math::AABB4i32 GetDesktopShape()
{
	win32::RECT rect = {};
	win32::BOOL success = win32::SystemParametersInfoA( SPI_GETWORKAREA, 0, &rect, 0 );
	RF_ASSERT( success == win32::TRUE );

	return details::AABBFromRect( rect );
}



PLATFORMUTILS_API math::AABB4i32 GetWindowShape( shim::HWND hWnd )
{
	win32::RECT rect;
	win32::BOOL const success = win32::GetClientRect( static_cast<win32::HWND>( hWnd ), &rect );
	RF_ASSERT( success == win32::TRUE );

	return details::AABBFromRect( rect );
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
	win32::MSG msg = {};
	if( PeekMessageA( &msg, nullptr, 0, 0, PM_REMOVE ) )
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
			DispatchMessageA( &msg );
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
}
