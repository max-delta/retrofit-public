#include "stdafx.h"

#include "PPU/Texture.h"
#include "PPU/TextureManager.h"

#include "PlatformUtils_win32/windowing.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );



int main()
{
	using namespace RF;

	UniquePtr<gfx::DeviceInterface> graphics = DefaultCreator<gfx::SimpleGL>::Create();
	graphics->Initialize2DGraphics();

	shim::HWND hwnd = platform::windowing::CreateNewWindow( 640, 480, WndProc );
	graphics->AttachToWindow( hwnd );

	UniquePtr<gfx::TextureManager> textureManager = DefaultCreator<gfx::TextureManager>::Create();
	textureManager->AttachToDevice( graphics );

	textureManager->LoadNewTexture( "Placeholder", "../../data/textures/common/max_delta_32.png" );
	WeakPtr<gfx::Texture> placeholderTex = textureManager->GetDeviceTextureForRenderFromTextureName( "Placeholder" );

	graphics->SetBackgroundColor( 1, 0, 1, 1 );
	graphics->SetSurfaceSize( 640, 480 );
	while( platform::windowing::ProcessMessages() )
	{
		graphics->BeginFrame();
		graphics->DrawBillboard( placeholderTex->GetDeviceRepresentation(), math::Vector2f( -.5f, .5f ), math::Vector2f( .5f, -.5f ), 0.f );
		graphics->RenderFrame();
		graphics->EndFrame();
	}
	textureManager->DestroyTexture( "Placeholder" );
	graphics->DetachFromWindow();
	graphics = nullptr;
	return 0;
}
