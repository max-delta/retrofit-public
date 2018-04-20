#include "stdafx.h"
#include "Tests.h"

#include "TEMP_AssetLoadSave.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/TextureManager.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "Scripting_squirrel/squirrel.h"

#include "core_platform/uuid.h"

#include <pugixml/pugixml.h>

#include "rftl/extension/static_array.h"
#include "rftl/sstream"


// TODO: Singleton manager
extern RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

namespace RF { namespace test {
///////////////////////////////////////////////////////////////////////////////

RF::gfx::Object testObj = {};
RF::gfx::Object testObj2 = {};
void InitDrawTest()
{
	using namespace RF;

	WeakPtr<gfx::TextureManager> texMan = g_Graphics->DebugGetTextureManager();
	WeakPtr<gfx::FramePackManager> framePackMan = g_Graphics->DebugGetFramePackManager();
	file::VFS& vfs = *file::VFS::HACK_GetInstance();

	file::VFSPath const commonFramepacks = file::VFS::k_Root.GetChild( "assets", "framepacks", "common" );

	UniquePtr<gfx::FramePackBase> digitFPack = LoadFramePackFromSquirrel( commonFramepacks.GetChild( "testdigit_loop.fpack.sq" ) );
	uint8_t const digitAnimationLength = digitFPack->CalculateTimeIndexBoundary();
	testObj.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack", rftl::move( digitFPack ) );
	testObj.m_MaxTimeIndex = digitAnimationLength;
	testObj.m_TimeSlowdown = 3;
	testObj.m_Looping = true;
	testObj.m_XCoord = gfx::k_TileSize * 2;
	testObj.m_YCoord = gfx::k_TileSize * 1;
	testObj.m_ZLayer = 0;

	UniquePtr<gfx::FramePackBase> wiggleFPack = LoadFramePackFromSquirrel( commonFramepacks.GetChild( "test64_wiggle.fpack.sq" ) );
	uint8_t const wiggleAnimationLength = wiggleFPack->CalculateTimeIndexBoundary();
	testObj2.m_FramePackID = framePackMan->LoadNewResourceGetID( "testpack2", rftl::move( wiggleFPack ) );
	testObj2.m_MaxTimeIndex = 4;
	testObj2.m_TimeSlowdown = 33 / 4;
	testObj2.m_Looping = true;;
	testObj2.m_XCoord = gfx::k_TileSize * 4;
	testObj2.m_YCoord = gfx::k_TileSize * 4;
	testObj2.m_ZLayer = 0;

	file::VFSPath const fonts = file::VFS::k_Root.GetChild( "assets", "textures", "fonts" );
	file::FileHandlePtr const fontHandle = vfs.GetFileForRead( fonts.GetChild( "font_narrow_1x.bmp" ) );
	g_Graphics->LoadFont( fontHandle->GetFile() );
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
	g_Graphics->DrawText( gfx::PPUCoord( 64, 64 + 8 * 0 ), gfx::PPUCoord( 4, 8 ), "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	g_Graphics->DrawText( gfx::PPUCoord( 64, 64 + 8 * 1 ), gfx::PPUCoord( 4, 8 ), "abcdefghijklmnopqrstuvwxyz" );
	g_Graphics->DrawText( gfx::PPUCoord( 64, 64 + 8 * 2 ), gfx::PPUCoord( 4, 8 ), "0123456789 !@#$%^&*()" );
	g_Graphics->DrawText( gfx::PPUCoord( 64, 64 + 8 * 3 ), gfx::PPUCoord( 4, 8 ), "`'\"~-=[]{}\\|,.<>/?" );
}



void DrawInputDebug()
{
	using namespace RF;

	rftl::string buf;
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
	rftl::stringstream logicStream;
	rftl::stringstream physStream;
	float signalValue;
	rftl::u16string textStream;
	rftl::string halfAsciid;

	g_Graphics->DebugDrawText( coord, "Input" );
	coord.y += offset;

	logicEvents.clear();
	g_WndProcInput->m_Digital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
	logicStream.clear();
	for( LogicEvents::value_type const& event : logicEvents )
	{
		logicStream <<
			" " <<
			static_cast<int>( event.m_Code ) <<
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
			static_cast<int>( event.m_Code ) <<
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



void SQTest()
{
	using RF::script::SquirrelVM;

	RF::script::SquirrelVM vm;
	constexpr wchar_t source[] =
		L"x <- 5;"
		L"y <- 7.0;"
		L"z <- true;"
		L"s <- \"STRING\";"
		L"n <- null;"
		L"a <- [\"first\", \"second\"];"
		L"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	RF_ASSERT( sourceAdd );

	SquirrelVM::Element xElem = vm.GetGlobalVariable( L"x" );
	RF_ASSERT( rftl::get_if<SquirrelVM::Integer>( &xElem ) != nullptr );
	SquirrelVM::Element yElem = vm.GetGlobalVariable( L"y" );
	RF_ASSERT( rftl::get_if<SquirrelVM::FloatingPoint>( &yElem ) != nullptr );
	SquirrelVM::Element zElem = vm.GetGlobalVariable( L"z" );
	RF_ASSERT( rftl::get_if<SquirrelVM::Boolean>( &zElem ) != nullptr );
	SquirrelVM::Element sElem = vm.GetGlobalVariable( L"s" );
	RF_ASSERT( rftl::get_if<SquirrelVM::String>( &sElem ) != nullptr );
	SquirrelVM::Element nElem = vm.GetGlobalVariable( L"n" );
	RF_ASSERT( rftl::get_if<SquirrelVM::Null>( &nElem ) != nullptr );
	SquirrelVM::Element aElem = vm.GetGlobalVariable( L"a" );
	RF_ASSERT( rftl::get_if<SquirrelVM::ArrayTag>( &aElem ) != nullptr );
	SquirrelVM::ElementArray aElemArr = vm.GetGlobalVariableAsArray( L"a" );
	{
		RF_ASSERT( aElemArr.size() == 2 );
		RF_ASSERT( rftl::get_if<SquirrelVM::String>( &aElemArr[0] ) != nullptr );
		RF_ASSERT( rftl::get_if<SquirrelVM::String>( &aElemArr[1] ) != nullptr );
	}
}



void XMLTest()
{
	file::VFS* vfs = file::VFS::HACK_GetInstance();
	file::VFSPath const testFilePath = file::VFS::k_Root.GetChild( "scratch", "xmltest.xml" );

	// Write
	{
		file::FileHandlePtr const testFile = vfs->GetFileForWrite( testFilePath );
		pugi::xml_writer_file writer{ testFile->GetFile() };

		pugi::xml_document doc;
		doc.append_child( "Header" ).append_attribute("Version") = 0;
		doc.append_child( "Data" );
		doc.save( writer );
		RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "May have written an XML file successfully?" );
	}

	// Read
	{
		file::FileHandlePtr const testFile = vfs->GetFileForRead( testFilePath );
		rftl::vector<uint8_t> initialFileContents;
		initialFileContents.resize( compiler::kMinPageSize, '\0' );
		size_t const elementsRead = fread( initialFileContents.data(), sizeof( uint8_t ), initialFileContents.size(), testFile->GetFile() );
		if( elementsRead > initialFileContents.size() )
		{
			RFLOG_FATAL( nullptr, RFCAT_STARTUPTEST, "File read overwrote buffer somehow" );
		}

		pugi::xml_document doc;
		pugi::xml_parse_result const parseResult = doc.load_buffer_inplace( initialFileContents.data(), elementsRead );
		if( parseResult )
		{
			RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "Read an XML file successfully" );
		}
		else
		{
			RFLOG_ERROR( nullptr, RFCAT_STARTUPTEST, "Failed to parse file that XML writer just wrote. Error: \"%s\"", parseResult.description() );
		}
	}

}



void FPackSerializationTest()
{
	WeakPtr<gfx::TextureManager> const texMan = g_Graphics->DebugGetTextureManager();

	// Load from squirrel
	file::VFSPath const commonFramepacks = file::VFS::k_Root.GetChild( "assets", "framepacks", "common" );
	rftl::string const rootFilename = "testdigit_loop";
	file::VFSPath const digitFPackPath = commonFramepacks.GetChild( rootFilename + ".fpack.sq" );
	UniquePtr<gfx::FramePackBase> const digitFPack = LoadFramePackFromSquirrel( digitFPackPath );

	// Serialize
	std::vector<uint8_t> buffer;
	bool const writeSuccess = gfx::FramePackSerDes::SerializeToBuffer( *texMan, buffer, *digitFPack );

	// Cleanup
	{
		size_t const numSlots = digitFPack->m_NumTimeSlots;
		gfx::FramePackBase::TimeSlot const* const timeSlots = digitFPack->GetTimeSlots();
		for( size_t i = 0; i < numSlots; i++ )
		{
			gfx::FramePackBase::TimeSlot const& timeSlot = timeSlots[i];
			if( timeSlot.m_TextureReference != gfx::k_InvalidManagedTextureID )
			{
				rftl::string const squirrelHack = file::VFS::CreateStringFromPath( digitFPackPath ).append( { static_cast<char>( i + 1 ), '\0' } );
				bool const cleanupSuccess = texMan->DestroyResource( squirrelHack );
				if( cleanupSuccess == false )
				{
					RFLOG_ERROR( digitFPackPath, RFCAT_STARTUPTEST, "Failed to cleanup squirrel hack from hack loading code" );
				}
			}
		}
	}

	if( writeSuccess == false )
	{
		RFLOG_ERROR( digitFPackPath, RFCAT_STARTUPTEST, "Failed to serialize FPack" );
		return;
	}

	// Deserialize
	rftl::vector<file::VFSPath> textures;
	UniquePtr<gfx::FramePackBase> framePack;
	bool const readSuccess = gfx::FramePackSerDes::DeserializeFromBuffer( textures, buffer, framePack );
	if( readSuccess == false )
	{
		RFLOG_ERROR( digitFPackPath, RFCAT_STARTUPTEST, "Failed to deserialize FPack" );
		return;
	}

	// Create file
	file::VFS const& vfs = *file::VFS::HACK_GetInstance();
	file::VFSPath const newFilePath = commonFramepacks.GetChild( rootFilename + ".fpack" );
	{
		file::FileHandlePtr const fileHandle = vfs.GetFileForWrite( newFilePath );
		if( fileHandle == nullptr )
		{
			RFLOG_ERROR( newFilePath, RFCAT_STARTUPTEST, "Failed to create FPack file" );
			return;
		}

		// Write file
		FILE* const file = fileHandle->GetFile();
		RF_ASSERT( file != nullptr );
		size_t const bytesWritten = fwrite( buffer.data(), sizeof( decltype( buffer )::value_type ), buffer.size(), file );
		RF_ASSERT( bytesWritten == buffer.size() );
	}

	// Load file
	gfx::FramePackManager& fpackMan = *g_Graphics->DebugGetFramePackManager();
	rftl::string const newFilename = file::VFS::CreateStringFromPath( newFilePath );
	bool const fpackLoadSuccess = fpackMan.LoadNewResource( newFilename, newFilePath );
	if( fpackLoadSuccess == false )
	{
		RFLOG_ERROR( newFilePath, RFCAT_STARTUPTEST, "Failed to load FPack file" );
		return;
	}

	// Cleanup
	fpackMan.DestroyResource( newFilename );
}



void PlatformTest()
{
	using namespace ::RF::platform;

	rftl::static_array<Uuid, 10> uuids;
	for(size_t i = 0; i < uuids.max_size(); i++)
	{
		Uuid const newUuid = Uuid::GenerateNewUuid();
		RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "Generated new UUID: %s", newUuid.GetDebugString().c_str() );
		uuids.emplace_back( newUuid );
	}
	uuids.clear();
}

///////////////////////////////////////////////////////////////////////////////
}}
