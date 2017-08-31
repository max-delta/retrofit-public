#include "stdafx.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "SimpleGL/SimpleGL.h"

#include "core_platform/windows_inc.h"
#include "core/ptr/unique_ptr.h"
#include "core/macros.h"

RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;



// Window Procedure
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam )
{
	bool inputIntercepted = false;
	shim::LRESULT inputResult = 0;
	RF::input::WndProcInputDevice* const inputDevice = g_WndProcInput;
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
							   // Lets us tell the window the limits that it can be resized to.
			( *( ( win32::MINMAXINFO* )lParam ) ).ptMinTrackSize.x = 640 * 1 + win32::GetSystemMetrics( SM_CXFRAME ) * 2;
			( *( ( win32::MINMAXINFO* )lParam ) ).ptMinTrackSize.y = 480 * 1 + win32::GetSystemMetrics( SM_CYFRAME ) * 2 + win32::GetSystemMetrics( SM_CYCAPTION );
			( *( ( win32::MINMAXINFO* )lParam ) ).ptMaxTrackSize.x = win32::GetSystemMetrics( SM_CXMAXTRACK );
			( *( ( win32::MINMAXINFO* )lParam ) ).ptMaxTrackSize.y = win32::GetSystemMetrics( SM_CYMAXTRACK );
			return 0;

		case WM_CLOSE: // No parameters
					   // The application is now closing.
			win32::PostQuitMessage( 0 ); // This value is passed as the wParam to the
										 // WM_QUIT message.
			return 0;

		case WM_DESTROY: // No parameters
						 // The application is now cleaning up.
			return 0;

		case WM_SIZE: // wParam: Sizing type | lParam(LO): width | lParam(HI): height
		{
			// LEGACY: Don't do this anymore! 
			// The size of the window has changed. Since we have certain
			// requirements as to how the game must be displayed, we will try to
			// guess the user's intentions and resize the window accordingly.
			//			int width = LOWORD( lParam );
			//			int height = HIWORD( lParam );
			//			float width_scale = float( width ) / float( 640 );
			//			float height_scale = float( height ) / float( 480 );
			//			float average_scale = ( ( width_scale - height_scale ) / 2.0f ) + height_scale;
			//			graphics.scale = int( average_scale );
			//			SetWindowPos( hWnd, NULL, 0, 0,
			//				graphics.screen_width*graphics.scale + GetSystemMetrics( SM_CXFRAME ) * 2,
			//				graphics.screen_height*graphics.scale + GetSystemMetrics( SM_CYFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION ),
			//				SWP_NOMOVE );
			//			graphics.SetupOpenGL(); // OpenGL must be re-initialized with the
			//									 // new size data.
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
