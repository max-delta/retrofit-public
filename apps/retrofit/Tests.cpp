#include "stdafx.h"
#include "Tests.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "AppCommon_GraphicalClient/FrameBuilder.h"
#include "AppCommon_GraphicalClient/StandardTaskScheduler.h"

#include "GameScripting/OOLoader.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/TextRows.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/FontRegistry.h"

#include "GameInput/RawInputController.h"
#include "GameInput/HotkeyController.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/TileLayerCSVLoader.h"
#include "PPU/TilesetManager.h"
#include "PPU/Tileset.h"
#include "PPU/FontManager.h"
#include "PPU/TextureManager.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "Scheduling/tasks/FunctorTask.h"
#include "RFType/CreateClassInfoDefinition.h"

#include "core_platform/uuid.h"
#include "core_platform/winuser_shim.h"
#include "core_rftype/stl_extensions/vector.h"
#include "core_rftype/stl_extensions/string.h"
#include "core/ptr/default_creator.h"

#include <pugixml/pugixml.h>

#include "rftl/extension/static_array.h"
#include "rftl/sstream"
#include "rftl/thread"


struct SQReflectTestContainedClass
{
	bool mBool;
};

struct SQReflectTestNestedClass
{
	bool mBool;
};

// NOTE: These declarations don't have to be in the root namespace, but the
//  meta info does, so it's simpler to just put them here since there's no
//  header, and hopping in and out of namespaces here would reduce readability
struct SQReflectTestClass
{
	bool mBool;
	void* mVoidPtr;
	RF::reflect::VirtualClass* mClassPtr;
	char mChar;
	wchar_t mWChar;
	char16_t mChar16;
	char32_t mChar32;
	float mFloat;
	double mDouble;
	long double mLongDouble;
	uint8_t mU8;
	int8_t mS8;
	uint16_t mU16;
	int16_t mS16;
	uint32_t mU32;
	int32_t mS32;
	uint64_t mU64;
	int64_t mS64;
	rftl::string mString;
	rftl::wstring mWString;
	rftl::vector<int32_t> mIntArray;
	SQReflectTestNestedClass mNested;
	rftl::vector<SQReflectTestContainedClass> mObjArray;
};

RFTYPE_CREATE_META( SQReflectTestContainedClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestContainedClass::mBool );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestContainedClass" );
}

RFTYPE_CREATE_META( SQReflectTestNestedClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestNestedClass::mBool );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestNestedClass" );
}

RFTYPE_CREATE_META( SQReflectTestClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestClass::mBool );
	RFTYPE_META().RawProperty( "mVoidPtr", &SQReflectTestClass::mVoidPtr );
	RFTYPE_META().RawProperty( "mClassPtr", &SQReflectTestClass::mClassPtr );
	RFTYPE_META().RawProperty( "mChar", &SQReflectTestClass::mChar );
	RFTYPE_META().RawProperty( "mWChar", &SQReflectTestClass::mWChar );
	RFTYPE_META().RawProperty( "mChar16", &SQReflectTestClass::mChar16 );
	RFTYPE_META().RawProperty( "mChar32", &SQReflectTestClass::mChar32 );
	RFTYPE_META().RawProperty( "mFloat", &SQReflectTestClass::mFloat );
	RFTYPE_META().RawProperty( "mDouble", &SQReflectTestClass::mDouble );
	RFTYPE_META().RawProperty( "mLongDouble", &SQReflectTestClass::mLongDouble );
	RFTYPE_META().RawProperty( "mU8", &SQReflectTestClass::mU8 );
	RFTYPE_META().RawProperty( "mS8", &SQReflectTestClass::mS8 );
	RFTYPE_META().RawProperty( "mU16", &SQReflectTestClass::mU16 );
	RFTYPE_META().RawProperty( "mS16", &SQReflectTestClass::mS16 );
	RFTYPE_META().RawProperty( "mU32", &SQReflectTestClass::mU32 );
	RFTYPE_META().RawProperty( "mS32", &SQReflectTestClass::mS32 );
	RFTYPE_META().RawProperty( "mU64", &SQReflectTestClass::mU64 );
	RFTYPE_META().RawProperty( "mS64", &SQReflectTestClass::mS64 );
	RFTYPE_META().ExtensionProperty( "mString", &SQReflectTestClass::mString );
	RFTYPE_META().ExtensionProperty( "mWString", &SQReflectTestClass::mWString );
	RFTYPE_META().ExtensionProperty( "mIntArray", &SQReflectTestClass::mIntArray );
	RFTYPE_META().ExtensionProperty( "mObjArray", &SQReflectTestClass::mObjArray );
	RFTYPE_META().RawProperty( "mNested", &SQReflectTestClass::mNested );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestClass" );
}


namespace RF { namespace test {
///////////////////////////////////////////////////////////////////////////////

static gfx::Object testObjDigit = {};
static gfx::Object testObjDigitFlips[3] = {};
static gfx::Object testObjWiggle = {};
static gfx::TileLayer testTileLayer = {};
static constexpr ui::FontPurposeID k1xFont = 1;
static constexpr ui::FontPurposeID k2xFont = 2;
void InitDrawTest()
{
	using namespace RF;

	gfx::PPUController& ppu = *app::gGraphics;
	gfx::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	gfx::FontManager const& fontMan = *ppu.GetFontManager();
	file::VFS& vfs = *app::gVfs;

	file::VFSPath const commonFramepacks = file::VFS::kRoot.GetChild( "assets", "framepacks", "common" );
	file::VFSPath const commonTilesets = file::VFS::kRoot.GetChild( "assets", "tilesets", "common" );
	file::VFSPath const commonTilemaps = file::VFS::kRoot.GetChild( "assets", "tilemaps", "common" );
	file::VFSPath const fonts = file::VFS::kRoot.GetChild( "assets", "fonts", "common" );

	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, commonFramepacks.GetChild( "testdigit_loop.fpack" ) );
	gfx::ManagedFramePackID const digitFPackID = framePackMan.GetManagedResourceIDFromResourceName( commonFramepacks.GetChild( "testdigit_loop.fpack" ) );
	WeakPtr<gfx::FramePackBase> digitFPack = framePackMan.GetResourceFromManagedResourceID( digitFPackID );
	uint8_t const digitAnimationLength = digitFPack->CalculateTimeIndexBoundary();
	testObjDigit.mFramePackID = digitFPackID;
	testObjDigit.mTimer.mMaxTimeIndex = digitAnimationLength;
	testObjDigit.mTimer.mTimeSlowdown = 3;
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


	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, commonFramepacks.GetChild( "test64_wiggle.fpack" ) );
	gfx::ManagedFramePackID const wiggleFPackID = framePackMan.GetManagedResourceIDFromResourceName( commonFramepacks.GetChild( "test64_wiggle.fpack" ) );
	WeakPtr<gfx::FramePackBase> wiggleFPack = framePackMan.GetResourceFromManagedResourceID( digitFPackID );
	uint8_t const wiggleAnimationLength = wiggleFPack->CalculateTimeIndexBoundary();
	testObjWiggle.mFramePackID = wiggleFPackID;
	testObjWiggle.mTimer.mMaxTimeIndex = 4;
	testObjWiggle.mTimer.mTimeSlowdown = 33 / 4;
	testObjWiggle.mLooping = true;
	testObjWiggle.mXCoord = gfx::kTileSize * 4;
	testObjWiggle.mYCoord = gfx::kTileSize * 4;
	testObjWiggle.mZLayer = 0;


	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, commonTilesets.GetChild( "pallete16_4.tset.txt" ) );
	testTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( commonTilesets.GetChild( "pallete16_4.tset.txt" ) );
	testTileLayer.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Quadruple;
	testTileLayer.mXCoord = 170;
	testTileLayer.mYCoord = 40;
	testTileLayer.mZLayer = 1;
	gfx::TileLayerCSVLoader::LoadTiles( testTileLayer, vfs, commonTilemaps.GetChild( "testhouse_10.csv" ) );

	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, fonts.GetChild( "font_narrow_2x.fnt.txt" ) );
	gfx::ManagedFontID const testFont1 = fontMan.GetManagedResourceIDFromResourceName( fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
	gfx::ManagedFontID const testFont2 = fontMan.GetManagedResourceIDFromResourceName( fonts.GetChild( "font_narrow_2x.fnt.txt" ) );

	app::gFontRegistry->RegisterFont( k1xFont, { testFont1, 8, 0, 1 } );
	app::gFontRegistry->RegisterFont( k2xFont, { testFont2, 8, 0, 2 } );
	app::gFontRegistry->RegisterFont( k2xFont, { testFont1, 8, 0, 1 } );
	app::gFontRegistry->RegisterFallbackFont( { testFont1, 8, 0, 1 } );
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
	testTileLayer.Animate();
	app::gGraphics->DrawTileLayer( testTileLayer );
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Test" );
	ui::Font const testFont1 = app::gFontRegistry->SelectBestFont( k1xFont, app::gGraphics->GetCurrentZoomFactor() );
	ui::Font const testFont2 = app::gFontRegistry->SelectBestFont( k2xFont, app::gGraphics->GetCurrentZoomFactor() );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 0 ), testFont1.mFontHeight, testFont1.mManagedFontID, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 1 ), testFont1.mFontHeight, testFont1.mManagedFontID, "abcdefghijklmnopqrstuvwxyz" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 2 ), testFont1.mFontHeight, testFont1.mManagedFontID, "0123456789 !@#$%^&*()" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 3 ), testFont1.mFontHeight, testFont1.mManagedFontID, "`'\"~-=[]{}\\|,.<>/?" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 4 ), testFont2.mFontHeight, testFont2.mManagedFontID, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 5 ), testFont2.mFontHeight, testFont2.mManagedFontID, "abcdefghijklmnopqrstuvwxyz" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 6 ), testFont2.mFontHeight, testFont2.mManagedFontID, "0123456789 !@#$%^&*()" );
	app::gGraphics->DrawText( gfx::PPUCoord( 192, 64 + 8 * 7 ), testFont2.mFontHeight, testFont2.mManagedFontID, "`'\"~-=[]{}\\|,.<>/?" );

	ui::Font const uncheckedTestFont1 = app::gFontRegistry->SelectBestFont( k1xFont, 50 );
	ui::Font const uncheckedTestFont2 = app::gFontRegistry->SelectBestFont( k2xFont, 50 );
	gfx::PPUCoordElem const monoPipeLen = app::gGraphics->CalculateStringLengthFormatted( uncheckedTestFont1.mFontHeight, uncheckedTestFont1.mManagedFontID, "|||" );
	gfx::PPUCoordElem const monoWLen = app::gGraphics->CalculateStringLengthFormatted( uncheckedTestFont1.mFontHeight, uncheckedTestFont1.mManagedFontID, "WWW" );
	gfx::PPUCoordElem const varPipeLen = app::gGraphics->CalculateStringLengthFormatted( uncheckedTestFont2.mFontHeight, uncheckedTestFont2.mManagedFontID, "|||" );
	gfx::PPUCoordElem const varWLen = app::gGraphics->CalculateStringLengthFormatted( uncheckedTestFont2.mFontHeight, uncheckedTestFont2.mManagedFontID, "WWW" );
	RF_ASSERT( monoPipeLen != 0 );
	RF_ASSERT( monoPipeLen == monoWLen );
	RF_ASSERT( varPipeLen != 0 );
	RF_ASSERT( varPipeLen != varWLen );
}



static bool sUseRawController = false;
static UniquePtr<input::RawInputController> sRawController;
static UniquePtr<input::HotkeyController> sHotkeyController;
void InitInputDebug()
{
	sRawController = DefaultCreator<input::RawInputController>::Create();

	input::RawInputController::LogicalMapping logicalMapping;
	logicalMapping[shim::VK_SPACE][input::DigitalPinState::Active] = 0;
	logicalMapping['W'][input::DigitalPinState::Active] = 1;
	logicalMapping['A'][input::DigitalPinState::Active] = 2;
	logicalMapping['S'][input::DigitalPinState::Active] = 3;
	logicalMapping['D'][input::DigitalPinState::Active] = 4;
	logicalMapping[shim::VK_UP][input::DigitalPinState::Active] = 1;
	logicalMapping[shim::VK_LEFT][input::DigitalPinState::Active] = 2;
	logicalMapping[shim::VK_DOWN][input::DigitalPinState::Active] = 3;
	logicalMapping[shim::VK_RIGHT][input::DigitalPinState::Active] = 4;
	sRawController->SetLogicalMapping( logicalMapping );

	input::RawInputController::SignalMapping signalMapping;
	signalMapping[input::WndProcAnalogInputComponent::k_CursorAbsoluteX] = 0;
	signalMapping[input::WndProcAnalogInputComponent::k_CursorAbsoluteY] = 1;
	sRawController->SetSignalMapping( signalMapping );

	sHotkeyController = DefaultCreator<input::HotkeyController>::Create();
	sHotkeyController->SetSource( sRawController );

	input::HotkeyController::CommandMapping commandMapping;
	commandMapping[0] = 100;
	commandMapping[1] = 101;
	commandMapping[3] = 103;
	sHotkeyController->SetCommandMapping( commandMapping );
}



void DrawInputDebug()
{
	gfx::PPUCoord coord( 32, 64 );
	gfx::PPUCoord::ElementType const offset = 16;

	app::gGraphics->DebugDrawText( coord, "Input" );
	coord.y += offset;

	if( sUseRawController == false )
	{
		rftl::string buf;
		using LogicalEvent = input::DigitalInputComponent::LogicalEvent;
		using PhysicalEvent = input::DigitalInputComponent::PhysicalEvent;
		using LogicEvents = rftl::static_array<LogicalEvent, 8>;
		using PhysicEvents = rftl::static_array<PhysicalEvent, 8>;
		using LogicEventParser = rftl::virtual_back_inserter_iterator<LogicalEvent, LogicEvents>;
		using PhysicEventParser = rftl::virtual_back_inserter_iterator<PhysicalEvent, PhysicEvents>;
		LogicEvents logicEvents;
		PhysicEvents physicEvents;
		LogicEventParser logicEventParser( logicEvents );
		PhysicEventParser physicEventParser( physicEvents );
		rftl::stringstream logicStream;
		rftl::stringstream physStream;
		float signalValue;
		rftl::u16string textStream;
		rftl::string halfAsciid;

		logicEvents.clear();
		app::gWndProcInput->mDigital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
		logicStream.str( "" );
		for( LogicEvents::value_type const& event : logicEvents )
		{
			logicStream << " " << static_cast<int>( event.mCode ) << ( event.mNewState == input::DigitalPinState::Active ? '#' : '-' );
		}
		app::gGraphics->DebugDrawText( coord, "  lev: %s", logicStream.str().c_str() );
		coord.y += offset;

		physicEvents.clear();
		app::gWndProcInput->mDigital.GetPhysicalEventStream( physicEventParser, physicEvents.max_size() );
		physStream.str( "" );
		for( PhysicEvents::value_type const& event : physicEvents )
		{
			physStream << " " << static_cast<int>( event.mCode ) << ( event.mNewState == input::DigitalPinState::Active ? '#' : '-' );
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
				halfAsciid.push_back( static_cast<char>( chr ) );
			}
			else
			{
				halfAsciid.push_back( '#' );
			}
		}
		app::gGraphics->DebugDrawText( coord, "  txt: %s", halfAsciid.c_str() );
		coord.y += offset;

		if( app::gWndProcInput->mDigital.WasActivatedLogical( shim::VK_F1 ) )
		{
			sUseRawController = true;
		}
	}
	else
	{
		using RawCommand = input::RawCommand;
		using RawCommands = rftl::static_array<RawCommand, 8>;
		using RawCommandParser = rftl::virtual_back_inserter_iterator<RawCommand, RawCommands>;
		using GameCommand = input::GameCommand;
		using GameCommands = rftl::static_array<GameCommand, 8>;
		using GameCommandParser = rftl::virtual_back_inserter_iterator<GameCommand, GameCommands>;
		using RawSignal = input::RawSignal;
		using RawSignals = rftl::static_array<RawSignal, 8>;
		using RawSignalSampler = rftl::virtual_back_inserter_iterator<RawSignal, RawSignals>;
		RawCommands rawCommands;
		RawCommandParser rawCommandParser( rawCommands );
		rftl::stringstream rawCommandStream;
		GameCommands gameCommands;
		GameCommandParser gameCommandParser( gameCommands );
		rftl::stringstream gameCommandStream;
		RawSignals rawSignals;
		RawSignalSampler rawSignalSampler( rawSignals );
		rftl::stringstream rawSignalStream;
		rftl::u16string rawTextStream;
		rftl::string rawHalfAsciid;

		sRawController->ConsumeInput( *app::gWndProcInput );

		rawCommands.clear();
		sRawController->GetRawCommandStream( rawCommandParser, rawCommands.max_size() );
		rawCommandStream.str( "" );
		for( RawCommands::value_type const& command : rawCommands )
		{
			rawCommandStream << " " << static_cast<int>( command.mType );
		}
		app::gGraphics->DebugDrawText( coord, "  rcmd: %s", rawCommandStream.str().c_str() );
		coord.y += offset;

		rawSignals.clear();
		sRawController->GetRawSignalStream( rawSignalSampler, rawSignals.max_size(), 0 );
		rawSignalStream.str( "" );
		for( RawSignals::value_type const& signal : rawSignals )
		{
			rawSignalStream << " " << static_cast<int>( signal.mValue );
		}
		app::gGraphics->DebugDrawText( coord, "  x: %s", rawSignalStream.str().c_str() );
		coord.y += offset;

		rawSignals.clear();
		sRawController->GetRawSignalStream( rawSignalSampler, rawSignals.max_size(), 1 );
		rawSignalStream.str( "" );
		for( RawSignals::value_type const& signal : rawSignals )
		{
			rawSignalStream << " " << static_cast<int>( signal.mValue );
		}
		app::gGraphics->DebugDrawText( coord, "  y: %s", rawSignalStream.str().c_str() );
		coord.y += offset;

		sRawController->GetTextStream( rawTextStream, 40 );
		rawHalfAsciid.clear();
		for( char16_t const& chr : rawTextStream )
		{
			if( chr <= 127 )
			{
				rawHalfAsciid.push_back( static_cast<char>( chr ) );
			}
			else
			{
				rawHalfAsciid.push_back( '#' );
			}
		}
		app::gGraphics->DebugDrawText( coord, "  txt: %s", rawHalfAsciid.c_str() );
		coord.y += offset;

		gameCommands.clear();
		sHotkeyController->GetGameCommandStream( gameCommandParser, gameCommands.max_size() );
		gameCommandStream.str( "" );
		for( GameCommands::value_type const& command : gameCommands )
		{
			gameCommandStream << " " << static_cast<int>( command.mType );
		}
		app::gGraphics->DebugDrawText( coord, "  gcmd: %s", gameCommandStream.str().c_str() );
		coord.y += offset;

		if( app::gWndProcInput->mDigital.WasActivatedLogical( shim::VK_F1 ) )
		{
			sUseRawController = false;
		}
	}
}



void InitUITest()
{
	gfx::PPUController& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::UIContext uiContext( uiManager );
	uiManager.SetRootRenderDepth( gfx::kNearestLayer + 50 );

	file::VFSPath const commonTilesets = file::VFS::kRoot.GetChild( "assets", "tilesets", "common" );

	// HACK: Slightly smaller, for testing
	uiManager.SetRootAABBReduction( gfx::kTileSize / 16 );
	uiManager.SetDebugAABBReduction( gfx::kTileSize / 16 );

	// Slice the root canvas
	constexpr bool kSlicesEnabled[9] = { false, false, true, false, false, false, true, false, true };
	WeakPtr<ui::controller::NineSlicer> const nineSlicer =
		uiManager.AssignStrongController(
			ui::kRootContainerID,
			DefaultCreator<ui::controller::NineSlicer>::Create(
				kSlicesEnabled ) );

	// Create some passthoughs, and then blow them up from the head
	WeakPtr<ui::controller::Passthrough> const passthrough8 =
		uiManager.AssignStrongController(
			nineSlicer->GetChildContainerID( 8 ),
			DefaultCreator<ui::controller::Passthrough>::Create() );
	WeakPtr<ui::controller::Passthrough> const passthrough8_1 =
		uiManager.AssignStrongController(
			passthrough8->GetChildContainerID(),
			DefaultCreator<ui::controller::Passthrough>::Create() );
	RF_ASSERT( passthrough8 != nullptr );
	RF_ASSERT( passthrough8_1 != nullptr );
	nineSlicer->DestroyChildContainer( uiManager, 8 );
	RF_ASSERT( passthrough8 == nullptr );
	RF_ASSERT( passthrough8_1 == nullptr );

	// Restore the container we blew up
	nineSlicer->CreateChildContainer( uiManager, 8 );

	// Add a text label
	WeakPtr<ui::controller::TextLabel> const textLabel8 =
		uiManager.AssignStrongController(
			nineSlicer->GetChildContainerID( 8 ),
			DefaultCreator<ui::controller::TextLabel>::Create() );
	textLabel8->SetFont( k2xFont );
	textLabel8->SetText( "TextLabel8 test" );
	textLabel8->SetColor( math::Color3f::kWhite );
	textLabel8->SetBorder( true );

	// Assign a label to confirm it's accessible
	uiManager.AssignLabel( nineSlicer->GetChildContainerID( 8 ), "TextLabel8" );
	WeakPtr<ui::Controller> const textLabel8Untyped = uiManager.GetMutableController( "TextLabel8" );
	RF_ASSERT( textLabel8 == textLabel8Untyped );
	WeakPtr<ui::controller::TextLabel> const textLabel8Casted = uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "TextLabel8" );
	RF_ASSERT( textLabel8Casted == textLabel8Untyped );

	// Add a frame
	WeakPtr<ui::controller::BorderFrame> const frame6 =
		uiManager.AssignStrongController(
			nineSlicer->GetChildContainerID( 6 ),
			DefaultCreator<ui ::controller::BorderFrame>::Create() );
	ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, commonTilesets.GetChild( "frame9_24.tset.txt" ) );
	gfx::ManagedTilesetID const frameTileset = tsetMan.GetManagedResourceIDFromResourceName( commonTilesets.GetChild( "frame9_24.tset.txt" ) );
	frame6->SetTileset( uiContext, frameTileset, { 4, 4 }, { 0, 0 } );

	// Put some text in the frame to make sure it's readable
	rftl::vector<rftl::string> frame6Text;
	frame6Text.emplace_back( "Line1" );
	frame6Text.emplace_back( "Line2" );
	frame6Text.emplace_back( "Line3" );
	frame6Text.emplace_back( "Line4" );
	frame6Text.emplace_back( "Line5" );
	WeakPtr<ui::controller::TextRows> const frame6TextRows =
		uiManager.AssignStrongController(
			frame6->GetChildContainerID(),
			DefaultCreator<ui::controller::TextRows>::Create(
				frame6Text.size(),
				k2xFont,
				ui::Justification::MiddleLeft,
				math::Color3f::kWhite ) );
	frame6TextRows->SetText( frame6Text );
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
	rftl::vector<uint8_t> buffer;
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
	rftl::string const newFilename = newFilePath.CreateString();
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
		auto func = []() -> void {
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
		auto func = []() -> void {
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
		auto func = []() -> void {
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
		rftl::this_thread::yield();
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



void SQReflectTest()
{
	script::OOLoader loader;

	// Inject
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestClass>( "SQReflectTestClass" );
		RF_ASSERT( inject );
	}
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestNestedClass>( "SQReflectTestNestedClass" );
		RF_ASSERT( inject );
	}
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestContainedClass>( "SQReflectTestContainedClass" );
		RF_ASSERT( inject );
	}

	// Load
	{
		constexpr char source[] =
			"x <- SQReflectTestClass();\n"
			"x.mBool = true;\n"
			"x.mVoidPtr = null;\n"
			"x.mClassPtr = null;\n"
			"x.mChar = 'a';\n"
			"x.mWChar = 'a';\n"
			"x.mChar16 = 'a';\n"
			"x.mChar32 = 'a';\n"
			"x.mFloat = 0.5;\n"
			"x.mDouble = 0.5;\n"
			"x.mLongDouble = 0.5;\n"
			"x.mU8 = 7;\n"
			"x.mS8 = 7;\n"
			"x.mU16 = 7;\n"
			"x.mS16 = 7;\n"
			"x.mU32 = 7;\n"
			"x.mS32 = 7;\n"
			"x.mU64 = 7;\n"
			"x.mS64 = 7;\n"
			"x.mString = \"test\";\n"
			"x.mWString = \"test\";\n"
			"x.mIntArray = [3,5,7];\n"
			"x.mObjArray = [ SQReflectTestContainedClass() ];\n"
			"x.mObjArray[0].mBool = true;\n"
			"x.mNested = SQReflectTestNestedClass();\n"
			"x.mNested.mBool = true;\n"
			"\n";
		bool const sourceAdd = loader.AddSourceFromBuffer( source, sizeof( source ) );
		RF_ASSERT( sourceAdd );
	}

	// Instantiate
	SQReflectTestClass testClass = {};

	// Populate
	bool const populateSuccess = loader.PopulateClass( "x", testClass );
	RF_ASSERT( populateSuccess );

	// Verify
	RF_ASSERT( testClass.mBool == true );
	RF_ASSERT( testClass.mVoidPtr == nullptr );
	RF_ASSERT( testClass.mClassPtr == nullptr );
	RF_ASSERT( testClass.mChar == 'a' );
	RF_ASSERT( testClass.mWChar == L'a' );
	RF_ASSERT( testClass.mChar16 == u'a' );
	RF_ASSERT( testClass.mChar32 == U'a' );
	RF_ASSERT( testClass.mFloat == 0.5f );
	RF_ASSERT( testClass.mDouble == 0.5 );
	RF_ASSERT( testClass.mLongDouble == 0.5l );
	RF_ASSERT( testClass.mU8 == 7 );
	RF_ASSERT( testClass.mS8 == 7 );
	RF_ASSERT( testClass.mU16 == 7 );
	RF_ASSERT( testClass.mS16 == 7 );
	RF_ASSERT( testClass.mU32 == 7 );
	RF_ASSERT( testClass.mS32 == 7 );
	RF_ASSERT( testClass.mU64 == 7 );
	RF_ASSERT( testClass.mS64 == 7 );
	RF_ASSERT( testClass.mString == "test" );
	RF_ASSERT( testClass.mWString == L"test" );
	RF_ASSERT( testClass.mIntArray.size() == 3 );
	RF_ASSERT( testClass.mIntArray[0] == 3 );
	RF_ASSERT( testClass.mIntArray[1] == 5 );
	RF_ASSERT( testClass.mIntArray[2] == 7 );
	RF_ASSERT( testClass.mObjArray.size() == 1 );
	RF_ASSERT( testClass.mObjArray[0].mBool == true );
	RF_ASSERT( testClass.mNested.mBool == true );
}

///////////////////////////////////////////////////////////////////////////////
}}
