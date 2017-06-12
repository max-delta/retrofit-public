#include "stdafx.h"

#include "PlatformUtils_win32/windowing.h"
#include "SimpleGL/SimpleGL.h"

#include "core_platform/windows_inc.h"

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );



int main()
{
	shim::HWND hwnd = RF::platform::windowing::CreateNewWindow(640,480,WndProc);
	RF::gfx::SimpleGL graphics;
	graphics.AttachToWindow( hwnd );

	graphics.SetBackgroundColor();
	graphics.SetSurfaceSize();
	while( RF::platform::windowing::ProcessMessages() )
	{
		graphics.BeginFrame();
		graphics.RenderFrame();
		graphics.EndFrame();
	}
	graphics.DetachFromWindow();
	return 0;
}
