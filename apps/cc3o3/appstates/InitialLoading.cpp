#include "stdafx.h"
#include "InitialLoading.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/appstates/AppStateRoute.h"
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


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct InitialLoading::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	bool mFirstFrame = true;
};

///////////////////////////////////////////////////////////////////////////////

void InitialLoading::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	// TODO: Setup UI
	app::gUiManager->RecreateRootContainer();
}



void InitialLoading::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void InitialLoading::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Loading..." );

	// HACK: Skip the first frame so we can get atleast 1 frame to render, then
	//  just load everything blocking
	// TODO: Asynchronous loading
	if( mInternalState->mFirstFrame )
	{
		mInternalState->mFirstFrame = false;
		return;
	}

	// TODO: Load stuff

	gfx::PPUController& ppu = *app::gGraphics;
	file::VFS const& vfs = *app::gVfs;

	// Load fonts
	{
		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, paths::CommonFonts().GetChild( "font_narrow_1x.fnt.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Font, paths::CommonFonts().GetChild( "font_narrow_2x.fnt.txt" ) );
		gfx::FontManager const& fontMan = *ppu.DebugGetFontManager();
		gfx::ManagedFontID const narrowFont1xMono = fontMan.GetManagedResourceIDFromResourceName( paths::CommonFonts().GetChild( "font_narrow_1x.fnt.txt" ) );
		gfx::ManagedFontID const narrowFont2xVari = fontMan.GetManagedResourceIDFromResourceName( paths::CommonFonts().GetChild( "font_narrow_2x.fnt.txt" ) );

		ui::FontRegistry& fontReg = *app::gFontRegistry;
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont1xMono, 8, 0, 1 } );
		fontReg.RegisterFont( ui::font::MinSize, { narrowFont2xVari, 8, 0, 2 } );
		fontReg.RegisterFont( ui::font::NarrowTileMono, { narrowFont1xMono, gfx::kTileSize, 0, 1 } );
		fontReg.RegisterFont( ui::font::NarrowHalfTileMono, { narrowFont1xMono, gfx::kTileSize / 2, 0, 1 } );
		fontReg.RegisterFont( ui::font::NarrowQuarterTileMono, { narrowFont1xMono, gfx::kTileSize / 4, 0, 1 } );
		fontReg.RegisterFont( ui::font::SmallMenuText, { narrowFont1xMono, 8, 1, 1 } );
		fontReg.RegisterFont( ui::font::SmallMenuText, { narrowFont2xVari, 8, 1, 2 } );
		fontReg.RegisterFont( ui::font::LargeMenuText, { narrowFont2xVari, 16, 3, 1 } );
		fontReg.RegisterFont( ui::font::SmallMenuSelection, { narrowFont1xMono, 8, 1, 1 } );
		fontReg.RegisterFont( ui::font::SmallMenuSelection, { narrowFont2xVari, 8, 1, 2 } );
		fontReg.RegisterFont( ui::font::LargeMenuSelection, { narrowFont2xVari, 16, 3, 1 } );
		fontReg.RegisterFont( ui::font::LargeMenuHeader, { narrowFont2xVari, 16, 3, 1 } );
		fontReg.RegisterFont( ui::font::MessageBox, { narrowFont2xVari, 16, 3, 1 } );
		fontReg.RegisterFallbackFont( { narrowFont1xMono, 8, 0, 1 } );
	}

	// Load tilesets
	// NOTE: Will cause associated textures to load
	{
		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "country_hills_back_96", paths::BackgroundTilesets().GetChild( "country_hills_back_96.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "country_hills_mid_32", paths::BackgroundTilesets().GetChild( "country_hills_mid_32.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "cloud_fog_a_512", paths::BackgroundTilesets().GetChild( "cloud_fog_a_512.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "cloud_fog_b_512", paths::BackgroundTilesets().GetChild( "cloud_fog_b_512.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "island_mock_64", paths::BackgroundTilesets().GetChild( "island_mock_64.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "elemgrid_2_4", paths::UITilesets().GetChild( "elemgrid_2_4.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "elemgrid_8_16", paths::UITilesets().GetChild( "elemgrid_8_16.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "elemgrid_10_80", paths::UITilesets().GetChild( "elemgrid_10_80.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "elemgrid_16_128", paths::UITilesets().GetChild( "elemgrid_16_128.tset.txt" ) );

		// TODO: Some kind of machinery for automating frames
		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "retro1_8_48", paths::DialogFrameTilesets().GetChild( "retro1_8_48.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "template_8_48", paths::DialogFrameTilesets().GetChild( "template_8_48.tset.txt" ) );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::Tileset, "wood_8_48", paths::DialogFrameTilesets().GetChild( "wood_8_48.tset.txt" ) );
	}

	// Load framepacks
	// NOTE: Will cause associated textures to load
	{
		// TODO: Defer load requests instead of forcing immediate load
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, "cc303_composite_192", paths::LogoFramepacks().GetChild( "cc303_composite_192.fpack" ) );
	}

	// Load localization
	{
		// TODO: Configurable language
		app::gLocEngine->InitializeFromKeymapFile( vfs, paths::Localizations().GetChild( "en_us.keymap.csv" ), loc::TextDirection::LeftToRight );
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
	{
		gElementDatabase->LoadDescTables(
			paths::ElementDescTables() );
		gElementDatabase->LoadTierUnlockTables(
			paths::ElementTierUnlockTables() );
	}

	context.mManager.RequestDeferredStateChange( GetStateAfterInitialLoad() );
}

///////////////////////////////////////////////////////////////////////////////
}}}
