#include "stdafx.h"

#include "PPU/PPUController.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PPU/FramePackManager.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"
#include "core_time/clocks.h"

#include "rftl/extension/static_array.h"

#include <sstream>
#include <thread>

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

// Global systems
RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
RF::UniquePtr<RF::file::VFS> g_Vfs;


constexpr bool drawInputDebug = true;
RF::gfx::Object testObj = {};
RF::gfx::Object testObj2 = {};
void InitDrawTest()
{
	using namespace RF;

	g_Vfs->DebugDumpMountTable();
	{
		file::FileHandlePtr testFile = g_Vfs->GetFileForWrite( file::VFS::k_Root.GetChild( "scratch", "test.txt" ) );
	}
	{
		file::FileHandlePtr testFile = g_Vfs->GetFileForRead( file::VFS::k_Root.GetChild( "scratch", "test.txt" ) );
	}

	WeakPtr<gfx::TextureManager> texMan = g_Graphics->DebugGetTextureManager();
	WeakPtr<gfx::FramePackManager> framePackMan = g_Graphics->DebugGetFramePackManager();

	// TODO: Cleanup
	file::VFSPath const commonTextures = file::VFS::k_Root.GetChild( "assets", "textures", "common" );
	UniquePtr<gfx::FramePack_512> testFramePack = DefaultCreator<gfx::FramePack_512>::Create();
	testFramePack->m_PreferredSlowdownRate = 3;
	testFramePack->m_NumTimeSlots = 10;
	testFramePack->m_TimeSlots[0].m_TextureReference = texMan->LoadNewTextureGetID( "test0", commonTextures.GetChild( "test0_32.png" ) );
	testFramePack->m_TimeSlots[1].m_TextureReference = texMan->LoadNewTextureGetID( "test1", commonTextures.GetChild( "test1_32.png" ) );
	testFramePack->m_TimeSlots[2].m_TextureReference = texMan->LoadNewTextureGetID( "test2", commonTextures.GetChild( "test2_32.png" ) );
	testFramePack->m_TimeSlots[3].m_TextureReference = texMan->LoadNewTextureGetID( "test3", commonTextures.GetChild( "test3_32.png" ) );
	testFramePack->m_TimeSlots[4].m_TextureReference = texMan->LoadNewTextureGetID( "test4", commonTextures.GetChild( "test4_32.png" ) );
	testFramePack->m_TimeSlots[5].m_TextureReference = texMan->LoadNewTextureGetID( "test5", commonTextures.GetChild( "test5_32.png" ) );
	testFramePack->m_TimeSlots[6].m_TextureReference = texMan->LoadNewTextureGetID( "test6", commonTextures.GetChild( "test6_32.png" ) );
	testFramePack->m_TimeSlots[7].m_TextureReference = texMan->LoadNewTextureGetID( "test7", commonTextures.GetChild( "test7_32.png" ) );
	testFramePack->m_TimeSlots[8].m_TextureReference = texMan->LoadNewTextureGetID( "test8", commonTextures.GetChild( "test8_32.png" ) );
	testFramePack->m_TimeSlots[9].m_TextureReference = texMan->LoadNewTextureGetID( "test9", commonTextures.GetChild( "test9_32.png" ) );
	testFramePack->m_TimeSlotSustains[0] = 11;
	testFramePack->m_TimeSlotSustains[1] = 11;
	testFramePack->m_TimeSlotSustains[2] = 11;
	testFramePack->m_TimeSlotSustains[3] = 11;
	testFramePack->m_TimeSlotSustains[4] = 11;
	testFramePack->m_TimeSlotSustains[5] = 11;
	testFramePack->m_TimeSlotSustains[6] = 11;
	testFramePack->m_TimeSlotSustains[7] = 11;
	testFramePack->m_TimeSlotSustains[8] = 11;
	testFramePack->m_TimeSlotSustains[9] = 11;
	uint8_t const testAnimationLength = testFramePack->CalculateTimeIndexBoundary();
	testObj.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack", std::move( testFramePack ) );
	testObj.m_MaxTimeIndex = testAnimationLength;
	testObj.m_TimeSlowdown = 3;
	testObj.m_Looping = true;
	testObj.m_XCoord = gfx::k_TileSize * 2;
	testObj.m_YCoord = gfx::k_TileSize * 1;
	testObj.m_ZLayer = 0;
	UniquePtr<gfx::FramePack_256> testFramePack2 = DefaultCreator<gfx::FramePack_256>::Create();
	testFramePack2->m_PreferredSlowdownRate = 33 / 4;
	testFramePack2->m_NumTimeSlots = 4;
	testFramePack2->m_TimeSlots[0].m_TextureReference = texMan->LoadNewTextureGetID( "testx_64", commonTextures.GetChild( "testx_64.png" ) );
	testFramePack2->m_TimeSlots[1].m_TextureReference = testFramePack2->m_TimeSlots[0].m_TextureReference;
	testFramePack2->m_TimeSlots[2].m_TextureReference = testFramePack2->m_TimeSlots[0].m_TextureReference;
	testFramePack2->m_TimeSlots[3].m_TextureReference = testFramePack2->m_TimeSlots[0].m_TextureReference;
	testFramePack2->m_TimeSlots[0].m_TextureOriginX = 0;
	testFramePack2->m_TimeSlots[0].m_TextureOriginY = 0;
	testFramePack2->m_TimeSlots[1].m_TextureOriginX = 1;
	testFramePack2->m_TimeSlots[1].m_TextureOriginY = 0;
	testFramePack2->m_TimeSlots[2].m_TextureOriginX = 1;
	testFramePack2->m_TimeSlots[2].m_TextureOriginY = 1;
	testFramePack2->m_TimeSlots[3].m_TextureOriginX = 0;
	testFramePack2->m_TimeSlots[3].m_TextureOriginY = 1;
	testObj2.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack2", std::move( testFramePack2 ) );
	testObj2.m_MaxTimeIndex = 4;
	testObj2.m_TimeSlowdown = 33 / 4;
	testObj2.m_Looping = true;;
	testObj2.m_XCoord = gfx::k_TileSize * 4;
	testObj2.m_YCoord = gfx::k_TileSize * 4;
	testObj2.m_ZLayer = 0;
}



void DrawTest()
{
	using namespace RF;

	g_Graphics->DebugDrawLine( gfx::PPUCoord( 32, 32 ), gfx::PPUCoord( 64, 64 ) );
	testObj.Animate();
	g_Graphics->DrawObject( testObj );
	testObj2.Animate();
	g_Graphics->DrawObject( testObj2 );
	g_Graphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Test" );
}



constexpr bool drawTest = true;
void DrawInputDebug()
{
	using namespace RF;

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

	g_Graphics->DebugDrawText( coord, "Input" );
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
	g_Graphics->DebugDrawText( coord, "  lev: %s", logicStream.str().c_str() );
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
	g_Graphics->DebugDrawText( coord, "  pev: %s", physStream.str().c_str() );
	coord.y += offset;

	signalValue = g_WndProcInput->m_Analog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteX );
	g_Graphics->DebugDrawText( coord, "  cax: %f", signalValue );
	coord.y += offset;
	signalValue = g_WndProcInput->m_Analog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteY );
	g_Graphics->DebugDrawText( coord, "  cay: %f", signalValue );
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
	g_Graphics->DebugDrawText( coord, "  txt: %s", halfAsciid.c_str() );
	coord.y += offset;
}



int main()
{
	using namespace RF;

	g_Vfs = DefaultCreator<file::VFS>::Create();
	bool const vfsInitialized = g_Vfs->AttemptInitialMount( "../../config/vfs_game.ini", "../../../rftest_user" );
	if( vfsInitialized == false )
	{
		RF_ASSERT_MSG( false, "Failed to startup VFS" );
		return 1;
	}
	file::VFS::HACK_SetInstance( g_Vfs );

	shim::HWND hwnd = platform::windowing::CreateNewWindow( 640, 480, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = EntwinedCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	g_Graphics = DefaultCreator<gfx::PPUController>::Create( std::move( renderDevice ) );
	g_Graphics->Initialize( 640, 480 );

	g_WndProcInput = EntwinedCreator<input::WndProcInputDevice>::Create();

	if( drawInputDebug )
	{
		InitDrawTest();
	}

	time::PerfClock::duration const desiredFrameTime = std::chrono::milliseconds( 33 );
	time::PerfClock::time_point frameStart = time::PerfClock::now();
	time::PerfClock::time_point frameEnd = time::PerfClock::now();
	while( platform::windowing::ProcessAllMessages() >= 0 )
	{
		time::PerfClock::time_point const naturalFrameEnd = time::PerfClock::now();
		time::PerfClock::duration const naturalFrameTime = naturalFrameEnd - frameStart;
		if( naturalFrameTime < desiredFrameTime )
		{
			time::PerfClock::duration const timeRemaining = desiredFrameTime - naturalFrameTime;
			std::this_thread::sleep_for( timeRemaining );
		}
		frameEnd = time::PerfClock::now();
		time::PerfClock::duration const frameTime = frameEnd - frameStart;
		time::FrameClock::add_time( frameTime );
		frameStart = time::PerfClock::now();

		g_Graphics->BeginFrame();
		{
			g_WndProcInput->OnTick();

			if( drawInputDebug )
			{
				DrawInputDebug();
			}

			if( drawTest )
			{
				DrawTest();
			}

			g_Graphics->SubmitToRender();
			g_Graphics->WaitForRender();
		}
		g_Graphics->EndFrame();
	}
	g_Graphics = nullptr;
	return 0;
}
