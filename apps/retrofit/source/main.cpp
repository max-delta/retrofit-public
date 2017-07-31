#include "stdafx.h"

#include "PlatformUtils_win32/windowing.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );



int main()
{
	shim::HWND hwnd = RF::platform::windowing::CreateNewWindow( 640, 480, WndProc );
	RF::UniquePtr<RF::gfx::DeviceInterface> graphics = RF::DefaultCreator<RF::gfx::SimpleGL>::Create();
	graphics->Initialize2DGraphics();
	graphics->AttachToWindow( hwnd );
	RF::gfx::SimpleGL::TextureID texture = graphics->LoadTexture( "../../data/textures/common/max_delta_32.png" );
	graphics->SetBackgroundColor( 1, 0, 1, 1 );
	graphics->SetSurfaceSize( 640, 480 );
	while( RF::platform::windowing::ProcessMessages() )
	{
		graphics->BeginFrame();
		graphics->DrawBillboard( texture, RF::math::Vector2f( -.5f, .5f ), RF::math::Vector2f( .5f, -.5f ), 0.f );
		graphics->RenderFrame();
		graphics->EndFrame();
	}
	graphics->UnloadTexture( texture );
	graphics->DetachFromWindow();
	graphics = nullptr;
	return 0;
}
