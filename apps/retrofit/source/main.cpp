#include "stdafx.h"

#include "PPU/PPUController.h"

#include "PlatformUtils_win32/windowing.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );



int main()
{
	using namespace RF;

	shim::HWND hwnd = platform::windowing::CreateNewWindow( 640, 480, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = DefaultCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	UniquePtr<gfx::PPUController> graphics = DefaultCreator<gfx::PPUController>::Create( std::move( renderDevice ) );
	graphics->Initialize( 640, 480 );

	while( platform::windowing::ProcessMessages() )
	{
		graphics->BeginFrame();
		{
			// TODO: Do stuff that affects state
			graphics->SubmitToRender();
			graphics->WaitForRender();
		}
		graphics->EndFrame();
	}
	graphics = nullptr;
	return 0;
}
