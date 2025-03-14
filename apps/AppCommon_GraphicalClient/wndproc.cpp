#include "stdafx.h"
#include "AppCommon_GraphicalClient/Common.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "SimpleGL/SimpleGL.h"

#include "PPU/PPUController.h"

#include "core_platform/inc/windows_inc.h"
#include "core_math/math_casts.h"
#include "core/ptr/unique_ptr.h"
#include "core/macros.h"

namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

// Window Procedure
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam )
{
	bool inputIntercepted = false;
	shim::LRESULT inputResult = 0;
	RF::input::WndProcInputDevice* const inputDevice = gWndProcInput;
	if( inputDevice != nullptr )
	{
		inputResult = inputDevice->ExamineTranslatedMessage( hWnd, message, wParam, lParam, inputIntercepted );
	}

	switch( message )
	{
		case WM_CREATE: // lParam: CREATESTRUCT*
			// Lets us provide data on how to build the window. Kinda pointless
			// since we already gave it all the data that CREATESTRUCT would
			// provide.
			return 0;

		case WM_GETMINMAXINFO: // lParam: MINMAXINFO*
		{
			// Lets us tell the window the limits that it can be resized to.
			int xPadding = win32::GetSystemMetrics( SM_CXFRAME ) * 2;
			int yPadding = win32::GetSystemMetrics( SM_CYFRAME ) * 2 + win32::GetSystemMetrics( SM_CYCAPTION );
			#if( _WIN32_WINNT >= 0x0600 )
			{
				// *grumble* This came in with VS2013 apparently
				xPadding += win32::GetSystemMetrics( SM_CXPADDEDBORDER );
			}
			#endif
			constexpr bool kAddMissingPixels = true;
			if( kAddMissingPixels )
			{
				// HACK: Somewhere, in the last 15 years, MS fucked this up.
				//  Everyone on the internet seems to just add random numbers
				//  to fix it, and nobody seems to know when it broke or how to
				//  actually handle it. General anecdotes seem to narrow this
				//  down to the XP->Vista timeframe.
				// TODO: I don't know... hope that someone eventually stumbles
				//  on the correct magic system metric combo?
				xPadding += 5;
				yPadding += 10;
			}
			( *( reinterpret_cast<win32::MINMAXINFO*>( lParam ) ) ).ptMinTrackSize.x = RF::gfx::ppu::kDesiredWidth * 1 + xPadding;
			( *( reinterpret_cast<win32::MINMAXINFO*>( lParam ) ) ).ptMinTrackSize.y = RF::gfx::ppu::kDesiredHeight * 1 + yPadding;
			( *( reinterpret_cast<win32::MINMAXINFO*>( lParam ) ) ).ptMaxTrackSize.x = win32::GetSystemMetrics( SM_CXMAXTRACK );
			( *( reinterpret_cast<win32::MINMAXINFO*>( lParam ) ) ).ptMaxTrackSize.y = win32::GetSystemMetrics( SM_CYMAXTRACK );
		}
			return 0;

		case WM_CLOSE: // No parameters
			// The application is now closing.
			win32::PostQuitMessage( 0 ); // This value is passed as the wParam to the
				// WM_QUIT message.
			return 0;

		case WM_DESTROY: // No parameters
			// The application is now cleaning up.
			return 0;

		case WM_SIZE: // wParam: Sizing type | lParam(LO): mWidth | lParam(HI): mHeight
		{
			// The size of the window has changed, let graphics know
			if( gGraphics != nullptr )
			{
				win32::RECT rect;
				win32::BOOL const success = win32::GetClientRect( static_cast<win32::HWND>( hWnd ), &rect );
				RF_ASSERT( success == win32::TRUE );
				gGraphics->ResizeSurface(
					RF::math::integer_cast<uint16_t>( rect.right ),
					RF::math::integer_cast<uint16_t>( rect.bottom ) );
			}
		}
			return 0;


		case WM_KEYDOWN: // wParam: Virtual Key Code | lParam: Miscellaneous extra information
			// A key that must be handled immediately has been pressed. Note
			// that all other keys are checked when needed.
			switch( wParam )
			{
				case VK_F1:
					// LEGACY: Code from a different use case, but need similar concept
					//  re-implemented
					//					graphics.LoadShaders();
					return 0;
				case VK_ESCAPE:
					win32::PostQuitMessage( 0 );
					return 0;
				default:
					break;
			}
			return 0;

		default: // Parameters vary
			// We really don't care what just happened, Windows can fuck with it
			// to its heart's desire.
			if( inputIntercepted )
			{
				return inputResult;
			}
			return shim::DefWindowProcW( hWnd, message, wParam, lParam );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
