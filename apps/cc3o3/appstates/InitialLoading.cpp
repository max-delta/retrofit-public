#include "stdafx.h"
#include "InitialLoading.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/casting/CastingEngine.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"
#include "GameSprite/CharacterCreator.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "PPU/FontManager.h"

#include "Localization/LocEngine.h"
#include "Localization/PageMapper.h"

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct InitialLoading::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	bool mLoadsQueued = false;
};

///////////////////////////////////////////////////////////////////////////////

void InitialLoading::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	RF_TODO_ANNOTATION( "Setup a UI of some kind for loading" );
	app::gUiManager->RecreateRootContainer();
}



void InitialLoading::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void InitialLoading::OnTick( AppStateTickContext& context )
{
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ppu.DebugDrawText( gfx::ppu::Coord( 32, 32 ), "Loading..." );

	if( mInternalState->mLoadsQueued )
	{
		if( ppu.HasOutstandingLoadRequests() == false )
		{
			// Finished loading, change state
			context.mManager.RequestDeferredStateChange( GetStateAfterInitialLoad() );
		}

		// Loads queued, don't queue again
		return;
	}

	file::VFS const& vfs = *app::gVfs;

	// Load fonts
	{
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Font, paths::CommonFonts().GetChild( "font_narrow_1x_mono.fnt.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Font, paths::CommonFonts().GetChild( "font_narrow_2x_mono.fnt.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Font, paths::CommonFonts().GetChild( "font_narrow_2x_vari.fnt.txt" ) );
		gfx::FontManager const& fontMan = *ppu.DebugGetFontManager();
		gfx::ManagedFontID const narrowFont1xMono = fontMan.GetManagedResourceIDFromResourceName( paths::CommonFonts().GetChild( "font_narrow_1x_mono.fnt.txt" ) );
		gfx::ManagedFontID const narrowFont2xMono = fontMan.GetManagedResourceIDFromResourceName( paths::CommonFonts().GetChild( "font_narrow_2x_mono.fnt.txt" ) );
		gfx::ManagedFontID const narrowFont2xVari = fontMan.GetManagedResourceIDFromResourceName( paths::CommonFonts().GetChild( "font_narrow_2x_vari.fnt.txt" ) );

		ui::FontRegistry& fontReg = *app::gFontRegistry;

		// Fallback
		fontReg.RegisterFallbackFont( { narrowFont1xMono, 8, 0, 1 } );

		// Procedural fonts, useful for debug or mockups, but shouldn't ship
		// NOTE: These don't respect baseline, so operate as raw blocks
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont1xMono, 8, 0, 1 } );
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont2xVari, 8, 0, 2 } );
		fontReg.RegisterFont( ui::font::NarrowTileMono, { narrowFont2xMono, gfx::ppu::kTileSize, 0, 1 } );
		fontReg.RegisterFont( ui::font::NarrowHalfTileMono, { narrowFont2xMono, gfx::ppu::kTileSize / 2, 0, 1 } );
		fontReg.RegisterFont( ui::font::NarrowQuarterTileMono, { narrowFont1xMono, gfx::ppu::kTileSize / 4, 0, 1 } );
		static_assert( gfx::ppu::kTileSize / 4 >= 8, "Tile size too small for smallest font" );

		// Normal fonts
		// NOTE: These use baselines, so may extend below the given y-coordinate
		ui::Font const mono8_1x{ narrowFont1xMono, 8, 1, 1 };
		ui::Font const vari8_2x{ narrowFont2xVari, 8, 1, 2 };
		ui::Font const mono16_1x{ narrowFont2xMono, 16, 2, 1 };
		ui::Font const vari16_1x{ narrowFont2xVari, 16, 3, 1 };

		// Normal fonts
		fontReg.RegisterFont( ui::font::SmallMenuText, mono8_1x );
		fontReg.RegisterFont( ui::font::SmallMenuText, vari8_2x );
		fontReg.RegisterFont( ui::font::LargeMenuText, vari16_1x );
		fontReg.RegisterFont( ui::font::SmallMenuSelection, mono8_1x );
		fontReg.RegisterFont( ui::font::SmallMenuSelection, vari8_2x );
		fontReg.RegisterFont( ui::font::LargeMenuSelection, vari16_1x );
		fontReg.RegisterFont( ui::font::LargeMenuHeader, vari16_1x );
		fontReg.RegisterFont( ui::font::ElementLabelMedium, mono8_1x );
		fontReg.RegisterFont( ui::font::ElementLabelMedium, vari8_2x );
		fontReg.RegisterFont( ui::font::ElementLabelFull, vari16_1x );
		fontReg.RegisterFont( ui::font::OverworldLabel, vari16_1x );
		fontReg.RegisterFont( ui::font::BattleName, mono8_1x );
		fontReg.RegisterFont( ui::font::BattleName, vari8_2x );
		fontReg.RegisterFont( ui::font::BattleHealth, mono16_1x );
		fontReg.RegisterFont( ui::font::BattleStamina, mono8_1x );
		fontReg.RegisterFont( ui::font::MessageBox, vari16_1x );
	}

	// Load tilesets
	// NOTE: Will cause associated textures to load
	{
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "country_hills_back_96", paths::BackgroundTilesets().GetChild( "country_hills_back_96.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "country_hills_mid_32", paths::BackgroundTilesets().GetChild( "country_hills_mid_32.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "elemgrid_2_4", paths::UITilesets().GetChild( "elemgrid_2_4.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "elemgrid_8_16", paths::UITilesets().GetChild( "elemgrid_8_16.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "elemgrid_10_80", paths::UITilesets().GetChild( "elemgrid_10_80.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "elemgrid_16_128", paths::UITilesets().GetChild( "elemgrid_16_128.tset.txt" ) );

		RF_TODO_ANNOTATION( "Some kind of machinery for automating frames" );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "flat1_8_48", paths::DialogFrameTilesets().GetChild( "flat1_8_48.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "flat2_8_48", paths::DialogFrameTilesets().GetChild( "flat2_8_48.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "retro1_8_48", paths::DialogFrameTilesets().GetChild( "retro1_8_48.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "retro2_8_48", paths::DialogFrameTilesets().GetChild( "retro2_8_48.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "template_8_48", paths::DialogFrameTilesets().GetChild( "template_8_48.tset.txt" ) );
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::Tileset, "wood_8_48", paths::DialogFrameTilesets().GetChild( "wood_8_48.tset.txt" ) );
	}

	// Load framepacks
	// NOTE: Will cause associated textures to load
	{
		ppu.QueueDeferredLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, "cc303_composite_192", paths::LogoFramepacks().GetChild( "cc303_composite_192.fpack" ) );
	}

	// Load localization
	{
		RF_TODO_ANNOTATION( "Make language configurable" );
		app::gLocEngine->InitializeFromKeymapDirectory( vfs, paths::Localizations().GetChild( "en_us" ), loc::TextDirection::LeftToRight );
		app::gPageMapper->InitializeFromCharmapFile( vfs, paths::Localizations().GetChild( "en_us.charmap.csv" ) );
	}

	// Load character creator
	{
		gCharacterCreator->LoadPieceTables(
			paths::CharacterTables().GetChild( "pieces" ) );
		gCharacterCreator->LoadCompositionTable(
			paths::CharacterTables().GetChild( "composite.csv" ) );
	}

	// Load character validator
	{
		gCharacterValidator->LoadGeneticsTable(
			paths::CharacterTables().GetChild( "genetics.csv" ) );
		gCharacterValidator->LoadStatBonusesTable(
			paths::CharacterTables().GetChild( "statbonuses.csv" ) );
	}

	// Load element database
	ReloadElementDatabase();

	// Load casting engine
	ReloadCastingEngine();

	mInternalState->mLoadsQueued = true;
}



void InitialLoading::ReloadElementDatabase()
{
	gElementDatabase->LoadDescTables(
		paths::ElementDescTables() );
	gElementDatabase->LoadTierUnlockTables(
		paths::ElementTierUnlockTables() );
}



void InitialLoading::ReloadCastingEngine()
{
	gCastingEngine->LoadActionDefinitions(
		paths::CastingActionDefinitions() );
}

///////////////////////////////////////////////////////////////////////////////
}
