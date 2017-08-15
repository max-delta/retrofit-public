#include "stdafx.h"

#include "PPU/PPUController.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PPU/FramePackManager.h"

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

	WeakPtr<gfx::TextureManager> texMan = graphics->DebugGetTextureManager();
	WeakPtr<gfx::FramePackManager> framePackMan = graphics->DebugGetFramePackManager();

	// TODO: Cleanup
	UniquePtr<gfx::FramePack_512> testFramePack = DefaultCreator<gfx::FramePack_512>::Create();
	testFramePack->m_PreferredSlowdownRate = 10;
	testFramePack->m_NumTimeSlots = 9;
	testFramePack->m_TimeSlots[0].m_TextureReference = texMan->LoadNewTextureGetID( "test0", "../../data/textures/common/test0_32.png" );
	testFramePack->m_TimeSlots[1].m_TextureReference = texMan->LoadNewTextureGetID( "test1", "../../data/textures/common/test1_32.png" );
	testFramePack->m_TimeSlots[2].m_TextureReference = texMan->LoadNewTextureGetID( "test2", "../../data/textures/common/test2_32.png" );
	testFramePack->m_TimeSlots[3].m_TextureReference = texMan->LoadNewTextureGetID( "test3", "../../data/textures/common/test3_32.png" );
	testFramePack->m_TimeSlots[4].m_TextureReference = texMan->LoadNewTextureGetID( "test4", "../../data/textures/common/test4_32.png" );
	testFramePack->m_TimeSlots[5].m_TextureReference = texMan->LoadNewTextureGetID( "test5", "../../data/textures/common/test5_32.png" );
	testFramePack->m_TimeSlots[6].m_TextureReference = texMan->LoadNewTextureGetID( "test6", "../../data/textures/common/test6_32.png" );
	testFramePack->m_TimeSlots[7].m_TextureReference = texMan->LoadNewTextureGetID( "test7", "../../data/textures/common/test7_32.png" );
	testFramePack->m_TimeSlots[8].m_TextureReference = texMan->LoadNewTextureGetID( "test8", "../../data/textures/common/test8_32.png" );
	testFramePack->m_TimeSlots[9].m_TextureReference = texMan->LoadNewTextureGetID( "test9", "../../data/textures/common/test9_32.png" );
	gfx::Object testObj = {};
	testObj.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack", std::move( testFramePack ) );
	testObj.m_TimeSlowdown = 10;
	testObj.m_XCoord = 0;
	testObj.m_YCoord = 0;
	testObj.m_ZLayer = 0;

	while( platform::windowing::ProcessMessages() )
	{
		graphics->BeginFrame();
		{
			// TODO: Do stuff that affects state
			graphics->DebugDrawLine( gfx::PPUCoord( 32, 32 ), gfx::PPUCoord( 64, 64 ) );
			testObj.Animate();
			graphics->DrawObject( testObj );

			graphics->SubmitToRender();
			graphics->WaitForRender();
		}
		graphics->EndFrame();
	}
	graphics = nullptr;
	return 0;
}
