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
#include "Scripting_squirrel/squirrel.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"
#include "core_math/math_casts.h"
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


RF::UniquePtr<RF::gfx::FramePackBase> LoadFramePackFromSquirrel( RF::file::VFSPath const& filename )
{
	using namespace RF;

	WeakPtr<gfx::TextureManager> texMan = g_Graphics->DebugGetTextureManager();
	script::SquirrelVM vm;
	script::SquirrelVM::Element elem;
	script::SquirrelVM::ElementArray elemArr;
	script::SquirrelVM::Integer const* integer;
	UniquePtr<gfx::FramePackBase> retVal;

	{
		std::wstring fileBuf;
		{
			file::FileHandlePtr const digitFPackFilePtr = g_Vfs->GetFileForRead( filename );
			RF_ASSERT( digitFPackFilePtr != nullptr );
			FILE* const digitFPackFile = digitFPackFilePtr->GetFile();
			RF_ASSERT( digitFPackFile != nullptr );

			long filesize;
			{
				fseek( digitFPackFile, 0, SEEK_END );
				filesize = ftell( digitFPackFile );
				rewind( digitFPackFile );
			}
			fileBuf.reserve( filesize );

			int fch = 0;
			while( ( fch = fgetc( digitFPackFile ) ) != EOF )
			{
				fileBuf.push_back( math::integer_cast<wchar_t>( fch ) );
			}
		}

		bool const sourceSuccess = vm.AddSourceFromBuffer( fileBuf );
		RF_ASSERT( sourceSuccess );
	}

	elem = vm.GetGlobalVariable( L"NumTimeSlots" );
	integer = std::get_if<script::SquirrelVM::Integer>( &elem );
	RF_ASSERT( integer != nullptr );
	if( *integer <= gfx::FramePack_256::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_256>::Create();
	}
	else if( *integer <= gfx::FramePack_512::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_512>::Create();
	}
	else if( *integer <= gfx::FramePack_1024::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_1024>::Create();
	}
	else if( *integer <= gfx::FramePack_4096::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_4096>::Create();
	}
	else
	{
		RF_ASSERT( false );
	}
	retVal->m_NumTimeSlots = math::integer_cast<uint8_t>( *integer );

	elem = vm.GetGlobalVariable( L"PreferredSlowdownRate" );
	integer = std::get_if<script::SquirrelVM::Integer>( &elem );
	RF_ASSERT( integer != nullptr );
	retVal->m_PreferredSlowdownRate = math::integer_cast<uint8_t>( *integer );

	elemArr = vm.GetGlobalVariableAsArray( L"Texture" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		std::wstring const* wstring = std::get_if<script::SquirrelVM::String>( &elemRef );
		RF_ASSERT( wstring != nullptr );
		std::string string;
		for( wchar_t const& wch : *wstring )
		{
			string.push_back( math::integer_cast<char>( wch ) );
		}
		retVal->GetMutableTimeSlots()[i].m_TextureReference =
			texMan->LoadNewTextureGetID(
				file::VFS::CreateStringFromPath( filename ).append( { (char)( i + 1 ),'\0' } ),
				file::VFS::k_Root.GetChild( file::VFS::CreatePathFromString( string ) ) );
	}

	elemArr = vm.GetGlobalVariableAsArray( L"Sustain" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		integer = std::get_if<script::SquirrelVM::Integer>( &elemRef );
		RF_ASSERT( integer != nullptr );
		retVal->GetMutableTimeSlotSustains()[i] = math::integer_cast<uint8_t>( *integer );
	}

	return retVal;
}



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

	file::VFSPath const commonFramepacks = file::VFS::k_Root.GetChild( "assets", "framepacks", "common" );
	file::VFSPath const fileName = commonFramepacks.GetChild( "testdigit_loop.fpack.sq" );
	UniquePtr<gfx::FramePackBase> digitFPack = LoadFramePackFromSquirrel( fileName );
	uint8_t const testAnimationLength = digitFPack->CalculateTimeIndexBoundary();
	testObj.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack", std::move( digitFPack ) );
	testObj.m_MaxTimeIndex = testAnimationLength;
	testObj.m_TimeSlowdown = 3;
	testObj.m_Looping = true;
	testObj.m_XCoord = gfx::k_TileSize * 2;
	testObj.m_YCoord = gfx::k_TileSize * 1;
	testObj.m_ZLayer = 0;

	// TODO: Cleanup
	file::VFSPath const commonTextures = file::VFS::k_Root.GetChild( "assets", "textures", "common" );
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



constexpr bool squirrelTest = true;
void SQTest()
{
	using RF::script::SquirrelVM;

	RF::script::SquirrelVM vm;
	wchar_t source[] =
		L"x <- 5;"
		L"y <- 7.0;"
		L"z <- true;"
		L"s <- \"STRING\";"
		L"n <- null;"
		L"a <- [\"first\", \"second\"];"
		L"\n";
	bool sourceAdd = vm.AddSourceFromBuffer( source );

	SquirrelVM::Element xElem = vm.GetGlobalVariable( L"x" );
	SquirrelVM::Integer* x = std::get_if<SquirrelVM::Integer>( &xElem );
	RF_ASSERT( x != nullptr );
	SquirrelVM::Element yElem = vm.GetGlobalVariable( L"y" );
	SquirrelVM::FloatingPoint* y = std::get_if<SquirrelVM::FloatingPoint>( &yElem );
	RF_ASSERT( y != nullptr );
	SquirrelVM::Element zElem = vm.GetGlobalVariable( L"z" );
	SquirrelVM::Boolean* z = std::get_if<SquirrelVM::Boolean>( &zElem );
	RF_ASSERT( z != nullptr );
	SquirrelVM::Element sElem = vm.GetGlobalVariable( L"s" );
	SquirrelVM::String* s = std::get_if<SquirrelVM::String>( &sElem );
	RF_ASSERT( s != nullptr );
	SquirrelVM::Element nElem = vm.GetGlobalVariable( L"n" );
	SquirrelVM::Null* n = std::get_if<SquirrelVM::Null>( &nElem );
	RF_ASSERT( n != nullptr );
	SquirrelVM::Element aElem = vm.GetGlobalVariable( L"a" );
	SquirrelVM::ArrayTag* a = std::get_if<SquirrelVM::ArrayTag>( &aElem );
	RF_ASSERT( a != nullptr );
	SquirrelVM::ElementArray aElemArr = vm.GetGlobalVariableAsArray( L"a" );
	{
		RF_ASSERT( aElemArr.size() == 2 );
		SquirrelVM::String* s1 = std::get_if<SquirrelVM::String>( &aElemArr[0] );
		RF_ASSERT( s != nullptr );
		SquirrelVM::String* s2 = std::get_if<SquirrelVM::String>( &aElemArr[1] );
		RF_ASSERT( s != nullptr );
	}
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

	if( squirrelTest )
	{
		SQTest();
	}

	shim::HWND hwnd = platform::windowing::CreateNewWindow( 640, 480, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = EntwinedCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	g_Graphics = DefaultCreator<gfx::PPUController>::Create( std::move( renderDevice ) );
	g_Graphics->Initialize( 640, 480 );

	g_WndProcInput = EntwinedCreator<input::WndProcInputDevice>::Create();

	if( drawTest )
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
