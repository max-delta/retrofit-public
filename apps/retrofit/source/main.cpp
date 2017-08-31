#include "stdafx.h"

#include "PPU/PPUController.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PPU/FramePackManager.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/static_array.h"

#include <sstream>

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;



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

	g_WndProcInput = DefaultCreator<input::WndProcInputDevice>::Create();

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

	while( platform::windowing::ProcessAllMessages() >= 0 )
	{
		graphics->BeginFrame();
		{
			g_WndProcInput->OnTick();

			constexpr bool drawInputDebug = true;
			if( drawInputDebug )
			{
				std::string buf;
				gfx::PPUCoord coord( 32, 64 );
				gfx::PPUCoord::ElementType const offset = 16;
				typedef input::DigitalInputComponent::LogicalEvent LogicalEvent;
				typedef input::DigitalInputComponent::PhysicalEvent PhysicalEvent;
				typedef rftl::static_array<LogicalEvent, 8> LogicEvents;
				typedef rftl::static_array<PhysicalEvent, 8> PhysicEvents;
				typedef input::BufferCopyEventParser<LogicalEvent, LogicEvents> LogicEventParser;
				typedef input::BufferCopyEventParser<PhysicalEvent, PhysicEvents> PhysicEventParser;
				LogicEvents logicEvents;
				PhysicEvents physicEvents;
				LogicEventParser logicEventParser( logicEvents );
				PhysicEventParser physicEventParser( physicEvents );
				std::stringstream logicStream;
				std::stringstream physStream;
				float signalValue;
				std::u16string textStream;
				std::string halfAsciid;

				graphics->DebugDrawText( coord, "Input" );
				coord.y += offset;

				logicEvents.clear();
				g_WndProcInput->m_Digital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
				logicStream.clear();
				for( LogicEvents::value_type const& event : logicEvents )
				{
					logicStream <<
						" " <<
						(int)event.m_Code <<
						( event.m_NewState == input::DigitalInputComponent::PinState::Active ? '#' : '-' );
				}
				graphics->DebugDrawText( coord, "  lev: %s", logicStream.str().c_str() );
				coord.y += offset;

				physicEvents.clear();
				g_WndProcInput->m_Digital.GetPhysicalEventStream( physicEventParser, physicEvents.max_size() );
				physStream.clear();
				for( PhysicEvents::value_type const& event : physicEvents )
				{
					physStream <<
						" " <<
						(int)event.m_Code <<
						( event.m_NewState == input::DigitalInputComponent::PinState::Active ? '#' : '-' );
				}
				graphics->DebugDrawText( coord, "  pev: %s", physStream.str().c_str() );
				coord.y += offset;

				signalValue = g_WndProcInput->m_Analog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteX );
				graphics->DebugDrawText( coord, "  cax: %f", signalValue );
				coord.y += offset;
				signalValue = g_WndProcInput->m_Analog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteY );
				graphics->DebugDrawText( coord, "  cay: %f", signalValue );
				coord.y += offset;

				g_WndProcInput->m_Text.GetTextStream( textStream, 100 );
				halfAsciid.clear();
				for( char16_t const& chr : textStream )
				{
					if( chr <= 127 )
					{
						halfAsciid.push_back( (char)chr );
					}
					else
					{
						halfAsciid.push_back( '#' );
					}
				}
				graphics->DebugDrawText( coord, "  txt: %s", halfAsciid.c_str() );
				coord.y += offset;
			}

			constexpr bool drawTest = true;
			if( drawTest )
			{
				graphics->DebugDrawLine( gfx::PPUCoord( 32, 32 ), gfx::PPUCoord( 64, 64 ) );
				testObj.Animate();
				graphics->DrawObject( testObj );
				graphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Test" );
			}

			graphics->SubmitToRender();
			graphics->WaitForRender();
		}
		graphics->EndFrame();
	}
	graphics = nullptr;
	return 0;
}
