#include "stdafx.h"

#include "PlatformUtils_win32/windowing.h"
#include "SimpleGL/SimpleGL.h"

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );



int main()
{
	shim::HWND hwnd = RF::platform::windowing::CreateNewWindow(640,480,WndProc);
	RF::gfx::SimpleGL graphics;
	graphics.AttachToWindow( hwnd );
	unsigned int texture = graphics.LoadTexture( "../../data/textures/common/max_delta_32.png" );
	graphics.SetProjectionMode( RF::gfx::SimpleGL::ProjectionMode::NDC11_11UPRIGHT );
	graphics.SetBackgroundColor();
	graphics.SetSurfaceSize();
	while( RF::platform::windowing::ProcessMessages() )
	{
		graphics.BeginFrame();
		graphics.DrawBillboard( texture, RF::gfx::Vector2f( -.5f, .5f ), RF::gfx::Vector2f( .5f, -.5f ), 0.f );
		graphics.RenderFrame();
		graphics.EndFrame();
	}
	graphics.UnloadTexture( texture );
	graphics.DetachFromWindow();
	return 0;
}
