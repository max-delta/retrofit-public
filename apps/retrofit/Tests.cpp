#include "stdafx.h"
#include "Tests.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "AppCommon_GraphicalClient/FrameBuilder.h"
#include "AppCommon_GraphicalClient/StandardTaskScheduler.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/TextureManager.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "Scheduling/tasks/FunctorTask.h"

#include "core_platform/uuid.h"
#include "core/ptr/default_creator.h"

#include <pugixml/pugixml.h>

#include "rftl/extension/static_array.h"
#include "rftl/sstream"
#include "rftl/thread"


namespace RF { namespace test {
///////////////////////////////////////////////////////////////////////////////

RF::gfx::Object testObjDigit = {};
RF::gfx::Object testObjDigitFlips[3] = {};
RF::gfx::Object testObjWiggle = {};
void InitDrawTest()
{
	using namespace RF;

	gfx::PPUController& ppu = *app::gGraphics;
	gfx::FramePackManager& framePackMan = *ppu.DebugGetFramePackManager();
	file::VFS& vfs = *app::gVfs;

	file::VFSPath const commonFramepacks = file::VFS::kRoot.GetChild( "assets", "framepacks", "common" );

	gfx::ManagedFramePackID const digitFPackID = framePackMan.LoadNewResourceGetID( "testpack", commonFramepacks.GetChild( "testdigit_loop.fpack" ) );
	WeakPtr<gfx::FramePackBase> digitFPack = framePackMan.GetResourceFromManagedResourceID( digitFPackID );
	uint8_t const digitAnimationLength = digitFPack->CalculateTimeIndexBoundary();
	testObjDigit.mFramePackID = digitFPackID;
	testObjDigit.mMaxTimeIndex = digitAnimationLength;
	testObjDigit.mTimeSlowdown = 3;
	testObjDigit.mLooping = true;
	testObjDigit.mXCoord = gfx::kTileSize * 2;
	testObjDigit.mYCoord = gfx::kTileSize * 1;
	testObjDigit.mZLayer = 0;

	testObjDigitFlips[0] = testObjDigit;
	testObjDigitFlips[0].mXCoord += gfx::kTileSize * 1;
	testObjDigitFlips[0].mHorizontalFlip = true;
	testObjDigitFlips[1] = testObjDigit;
	testObjDigitFlips[1].mXCoord += gfx::kTileSize * 2;
	testObjDigitFlips[1].mVerticalFlip = true;
	testObjDigitFlips[2] = testObjDigit;
	testObjDigitFlips[2].mXCoord += gfx::kTileSize * 3;
	testObjDigitFlips[2].mHorizontalFlip = true;
	testObjDigitFlips[2].mVerticalFlip = true;


	gfx::ManagedFramePackID const wiggleFPackID = framePackMan.LoadNewResourceGetID( "testpack2", commonFramepacks.GetChild( "test64_wiggle.fpack" ) );
	WeakPtr<gfx::FramePackBase> wiggleFPack = framePackMan.GetResourceFromManagedResourceID( digitFPackID );
	uint8_t const wiggleAnimationLength = wiggleFPack->CalculateTimeIndexBoundary();
	testObjWiggle.mFramePackID = wiggleFPackID;
	testObjWiggle.mMaxTimeIndex = 4;
	testObjWiggle.mTimeSlowdown = 33 / 4;
	testObjWiggle.mLooping = true;
	testObjWiggle.mXCoord = gfx::kTileSize * 4;
	testObjWiggle.mYCoord = gfx::kTileSize * 4;
	testObjWiggle.mZLayer = 0;

	file::VFSPath const fonts = file::VFS::kRoot.GetChild( "assets", "textures", "fonts" );
	file::FileHandlePtr const fontHandle = vfs.GetFileForRead( fonts.GetChild( "font_narrow_1x.bmp" ) );
	app::gGraphics->LoadFont( fontHandle->GetFile() );
}



void DrawTest()
{
	using namespace RF;

	app::gGraphics->DebugDrawLine( gfx::PPUCoord( 32, 32 ), gfx::PPUCoord( 64, 64 ) );
	testObjDigit.Animate();
	app::gGraphics->DrawObject( testObjDigit );
	for( size_t i = 0; i < rftl::extent<decltype( testObjDigitFlips )>::value; i++ )
	{
		testObjDigitFlips[i].Animate();
		app::gGraphics->DrawObject( testObjDigitFlips[i] );
	}
	testObjWiggle.Animate();
	app::gGraphics->DrawObject( testObjWiggle );
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Test" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 0 ), gfx::PPUCoord( 4, 8 ), "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 1 ), gfx::PPUCoord( 4, 8 ), "abcdefghijklmnopqrstuvwxyz" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 2 ), gfx::PPUCoord( 4, 8 ), "0123456789 !@#$%^&*()" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 3 ), gfx::PPUCoord( 4, 8 ), "`'\"~-=[]{}\\|,.<>/?" );
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

	app::gGraphics->DebugDrawText( coord, "Input" );
	coord.y += offset;

	logicEvents.clear();
	app::gWndProcInput->mDigital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
	logicStream.clear();
	for( LogicEvents::value_type const& event : logicEvents )
	{
		logicStream <<
			" " <<
			static_cast<int>( event.m_Code ) <<
			( event.m_NewState == input::DigitalInputComponent::PinState::Active ? '#' : '-' );
	}
	app::gGraphics->DebugDrawText( coord, "  lev: %s", logicStream.str().c_str() );
	coord.y += offset;

	physicEvents.clear();
	app::gWndProcInput->mDigital.GetPhysicalEventStream( physicEventParser, physicEvents.max_size() );
	physStream.clear();
	for( PhysicEvents::value_type const& event : physicEvents )
	{
		physStream <<
			" " <<
			static_cast<int>( event.m_Code ) <<
			( event.m_NewState == input::DigitalInputComponent::PinState::Active ? '#' : '-' );
	}
	app::gGraphics->DebugDrawText( coord, "  pev: %s", physStream.str().c_str() );
	coord.y += offset;

	signalValue = app::gWndProcInput->mAnalog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteX );
	app::gGraphics->DebugDrawText( coord, "  cax: %f", signalValue );
	coord.y += offset;
	signalValue = app::gWndProcInput->mAnalog.GetCurrentSignalValue( input::WndProcAnalogInputComponent::k_CursorAbsoluteY );
	app::gGraphics->DebugDrawText( coord, "  cay: %f", signalValue );
	coord.y += offset;

	app::gWndProcInput->mText.GetTextStream( textStream, 100 );
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
	app::gGraphics->DebugDrawText( coord, "  txt: %s", halfAsciid.c_str() );
	coord.y += offset;
}



void XMLTest()
{
	file::VFS* vfs = app::gVfs;
	file::VFSPath const testFilePath = file::VFS::kRoot.GetChild( "scratch", "xmltest.xml" );

	// Write
	{
		file::FileHandlePtr const testFile = vfs->GetFileForWrite( testFilePath );
		pugi::xml_writer_file writer{ testFile->GetFile() };

		pugi::xml_document doc;
		doc.append_child( "Header" ).append_attribute( "Version" ) = 0;
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
	gfx::PPUController& ppu = *app::gGraphics;
	gfx::FramePackManager& fpackMan = *ppu.DebugGetFramePackManager();
	gfx::TextureManager& texMan = *ppu.DebugGetTextureManager();

	// Load a common test asset
	file::VFSPath const commonFramepacks = file::VFS::kRoot.GetChild( "assets", "framepacks", "common" );
	file::VFSPath const digitFPackPath = commonFramepacks.GetChild( "testdigit_loop.fpack" );
	gfx::ManagedFramePackID const digitFPackID = fpackMan.LoadNewResourceGetID( "sertestpack", digitFPackPath );
	WeakPtr<gfx::FramePackBase> const digitFPack = fpackMan.GetResourceFromManagedResourceID( digitFPackID );

	// Serialize
	std::vector<uint8_t> buffer;
	bool const writeSuccess = gfx::FramePackSerDes::SerializeToBuffer( texMan, buffer, *digitFPack );

	// Cleanup
	fpackMan.DestroyResource( "sertestpack" );

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
	file::VFS const& vfs = *app::gVfs;
	file::VFSPath const newFilePath = file::VFS::kRoot.GetChild( "scratch", "sertest.fpack" );
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
	for( size_t i = 0; i < uuids.max_size(); i++ )
	{
		Uuid const newUuid = Uuid::GenerateNewUuid();
		RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "Generated new UUID: %s", newUuid.GetDebugString().c_str() );
		uuids.emplace_back( newUuid );
	}
	uuids.clear();
}



static UniquePtr<app::FrameBuilder> gTestFrameBuilder;
static uint64_t gTestFrameBuilderFrameCount;
void InitFrameBuilderTest()
{
	gTestFrameBuilder = DefaultCreator<app::FrameBuilder>::Create( app::gTaskScheduler->GetTaskScheduler() );

	{
		auto func = []() -> void
		{
			RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "START running on thread hash %llu", rftl::hash<rftl::thread::id>()( rftl::this_thread::get_id() ) );
		};
		auto myFunctor = scheduling::CreateCloneableFunctorTask( rftl::move( func ) );
		using TaskType = decltype( myFunctor );
		UniquePtr<TaskType> newTask = DefaultCreator<TaskType>::Create( rftl::move( myFunctor ) );
		app::FrameBuilder::TaskRepresentation taskDef;
		taskDef.mMeta = newTask;
		taskDef.mPostconditions.mStates.emplace( "Test", "Started" );
		gTestFrameBuilder->AddTask( rftl::move( taskDef ), rftl::move( newTask ) );
	}
	{
		auto func = []() -> void
		{
			RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "PAIR1 running on thread hash %llu", rftl::hash<rftl::thread::id>()( rftl::this_thread::get_id() ) );
		};
		auto myFunctor = scheduling::CreateCloneableFunctorTask( rftl::move( func ) );
		using TaskType = decltype( myFunctor );
		UniquePtr<TaskType> newTask = DefaultCreator<TaskType>::Create( rftl::move( myFunctor ) );
		app::FrameBuilder::TaskRepresentation taskDef;
		taskDef.mMeta = newTask;
		taskDef.mPreconditions.mStates.emplace( "Test", "Started" );
		taskDef.mPostconditions.mStates.emplace( "Pair1", "Run" );
		gTestFrameBuilder->AddTask( rftl::move( taskDef ), rftl::move( newTask ) );
	}
	{
		auto func = []() -> void
		{
			RFLOG_TRACE( nullptr, RFCAT_STARTUPTEST, "PAIR2 running on thread hash %llu", rftl::hash<rftl::thread::id>()( rftl::this_thread::get_id() ) );
		};
		auto myFunctor = scheduling::CreateCloneableFunctorTask( rftl::move( func ) );
		using TaskType = decltype( myFunctor );
		UniquePtr<TaskType> newTask = DefaultCreator<TaskType>::Create( rftl::move( myFunctor ) );
		app::FrameBuilder::TaskRepresentation taskDef;
		taskDef.mMeta = newTask;
		taskDef.mPreconditions.mStates.emplace( "Test", "Started" );
		taskDef.mPostconditions.mStates.emplace( "Pair2", "Run" );
		gTestFrameBuilder->AddTask( rftl::move( taskDef ), rftl::move( newTask ) );
	}

	gTestFrameBuilder->FinalizePlan();

	gTestFrameBuilder->SetDesiredCondition( "Pair1", "Run" );
	gTestFrameBuilder->SetDesiredCondition( "Pair2", "Run" );

	gTestFrameBuilderFrameCount = 0;
}



void FrameBuilderTest()
{
	gTestFrameBuilder->StartPlan();
	while( gTestFrameBuilder->IsPlanComplete() == false )
	{
		// Spin
	}
	gTestFrameBuilderFrameCount++;
	if( gTestFrameBuilderFrameCount == 2 )
	{
		gTestFrameBuilder->RemoveDesiredCondition( "Pair1" );
	}
	else if( gTestFrameBuilderFrameCount == 4 )
	{
		gTestFrameBuilder->RemoveDesiredCondition( "Pair2" );
	}
}



void TerminateFrameBuilderTest()
{
	gTestFrameBuilder = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}
