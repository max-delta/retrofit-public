#include "stdafx.h"
#include "Gameplay.h"

#include "cc3o3/appstates/gameplay/Gameplay_Overworld.h"
#include "cc3o3/appstates/gameplay/Gameplay_Site.h"
#include "cc3o3/appstates/gameplay/Gameplay_Battle.h"
#include "cc3o3/appstates/gameplay/Gameplay_Menus.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/LocalUI.h"
#include "cc3o3/state/objects/Company.h"
#include "cc3o3/state/objects/BaseCharacter.h"
#include "cc3o3/state/objects/OverworldCharacter.h"
#include "cc3o3/state/objects/SiteCharacter.h"
#include "cc3o3/state/objects/BattleCharacter.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/Progression.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameAppState/AppStateTickContext.h"
#include "GameSprite/CharacterCreator.h"
#include "GameSync/SnapshotSerializer.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFSPath.h"
#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"
#include "PlatformFilesystem/VFS.h"

#include "core_component/TypedObjectManager.h"
#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"
#include "core_state/VariableIdentifier.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	AppStateManager mAppStateManager;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	// TODO: Multiplayer considerations and logic
	InputHelpers::SetSinglePlayer( input::player::P1 );
	input::HardcodedPlayerSetup( InputHelpers::GetSinglePlayer() );

	// TODO: Load save

	// Prepare the character database
	character::CharacterDatabase& charDB = *gCharacterDatabase;
	{
		// Reset
		charDB.DeleteAllCharacters();

		// Load base characters
		file::VFSPath const base = paths::BaseCharacters();
		size_t const charsLoaded = charDB.LoadFromPersistentStorage( base );
		RFLOG_INFO( base, RFCAT_CC3O3, "Loaded %llu chars", charsLoaded );
		RFLOG_TEST_AND_FATAL( charsLoaded > 0, base, RFCAT_CC3O3, "Failed to load any base characters" );

		// TODO: Load custom characters
		// NOTE: May override existing characters

		// TODO: Load characters from save
		// NOTE: May override existing characters
	}

	// Sanitize characters
	character::CharacterDatabase::CharacterIDs const characterIDs = charDB.GetAllCharacterIDs();
	{
		character::CharacterValidator const& charValidate = *gCharacterValidator;

		for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
		{
			character::CharData character = charDB.FetchExistingCharacter( characterID );
			charValidate.SanitizeForGameplay( character );
			charDB.OverwriteExistingCharacter( characterID, rftl::move( character ) );
		}
	}

	// Composite characters
	{
		sprite::CharacterCreator& charCreate = *gCharacterCreator;

		rftl::array<sprite::CompositeCharacterParams, 2> paramSets = {};
		paramSets.at( 0 ).mMode = "24";
		paramSets.at( 0 ).mCompositeWidth = 24;
		paramSets.at( 0 ).mCompositeHeight = 24;
		paramSets.at( 0 ).mCharPiecesDir = paths::CharacterPieces();
		paramSets.at( 1 ).mMode = "36";
		paramSets.at( 1 ).mCompositeWidth = 36;
		paramSets.at( 1 ).mCompositeHeight = 40;
		paramSets.at( 1 ).mCharPiecesDir = paths::CharacterPieces();

		for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
		{
			character::CharData const input = charDB.FetchExistingCharacter( characterID );

			for( sprite::CompositeCharacterParams const& paramSet : paramSets )
			{
				sprite::CompositeCharacterParams params = paramSet;
				params.mBaseId = input.mVisuals.mBase;
				params.mTopId = input.mVisuals.mTop;
				params.mBottomId = input.mVisuals.mBottom;
				params.mHairId = input.mVisuals.mHair;
				params.mSpeciesId = input.mVisuals.mSpecies;
				params.mOutputDir = paths::CompositeCharacters().GetChild( characterID ).GetChild( params.mMode );

				sprite::CompositeCharacter output = charCreate.CreateCompositeCharacter( params );
				charDB.SubmitOrOverwriteComposite( characterID, params.mMode, rftl::move( output ) );
			}
		}
	}

	// Set up objects
	{
		using namespace state;
		using namespace state::obj;

		rollback::RollbackManager& rollMan = *gRollbackManager;
		rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
		rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();
		rollback::Window& sharedWindow = sharedDomain.GetMutableWindow();
		rollback::Window& privateWindow = privateDomain.GetMutableWindow();

		// Set up each company
		// TODO: Multiple companies for competitive multiplayer
		{
			// HACK: Only local player
			input::PlayerID const playerID = InputHelpers::GetSinglePlayer();
			VariableIdentifier const companyRoot( "company", rftl::to_string( playerID ) );

			// Create company object
			MutableObjectRef const company = CreateCompany( sharedWindow, privateWindow, companyRoot );
			comp::Roster& roster = *company.GetMutableComponentInstanceT<comp::Roster>();
			comp::Progression& progression = *company.GetMutableComponentInstanceT<comp::Progression>();

			// Set up all characters
			// NOTE: Characters with the company as their root are implicitly
			//  on the company's roster
			// TODO: Data-driven list
			company::RosterIndex rosterIndex = company::kInititialRosterIndex;
			VariableIdentifier const charRoot = companyRoot.GetChild( "member" );
			for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
			{
				MutableObjectRef const newChar = CreateBaseCharacterFromDB(
					sharedWindow, privateWindow,
					charRoot.GetChild( rftl::to_string( rosterIndex ) ), characterID );
				MakeOverworldCharacterFromDB(
					sharedWindow, privateWindow,
					newChar, characterID );
				MakeSiteCharacterFromDB(
					sharedWindow, privateWindow,
					newChar, characterID );
				MakeBattleCharacterFromDB(
					sharedWindow, privateWindow,
					newChar, characterID );

				// HACK: All valid characters are eligible
				// TODO: Tie to story progression
				roster.mEligible.at( rosterIndex ) = true;

				rosterIndex++;
			}

			// HACK: Make sure some hard-coded characters are present
			// TODO: Check active team conditions, auto-fill if needed
			RF_ASSERT( roster.mEligible.at( 0 ) );
			RF_ASSERT( roster.mEligible.at( 1 ) );
			RF_ASSERT( roster.mEligible.at( 2 ) );
			roster.mActiveTeam.at( 0 ) = 1;
			roster.mActiveTeam.at( 1 ) = 2;
			roster.mActiveTeam.at( 2 ) = 0;

			// HACK: Set some fake progression
			progression.mStoryTier = 6;

			// Read in loadouts
			// TODO: Sanitize? Or atleast warn on invalid setup
			gCompanyManager->ReadLoadoutsFromSave( playerID );
		}

		// Set up the UI
		{
			VariableIdentifier const localUIRoot( "localUI" );
			CreateLocalUI( sharedWindow, privateWindow, localUIRoot );
		}

		// HACK: Save out the state for diagnostics
		// TODO: More formal snapshotting
		{
			WeakPtr<rollback::Snapshot const> const sharedSnapshot =
				sharedDomain.TakeManualSnapshot( "GameStart", time::FrameClock::now() );
			RF_ASSERT( sharedSnapshot != nullptr );
			WeakPtr<rollback::Snapshot const> const privateSnapshot =
				privateDomain.TakeManualSnapshot( "GameStart", time::FrameClock::now() );
			RF_ASSERT( privateSnapshot != nullptr );

			file::VFS const& vfs = *app::gVfs;
			sync::SnapshotSerializer::SerializeToDiagnosticFile(
				*sharedSnapshot, vfs, file::VFS::kRoot.GetChild( "scratch", "snapshots", "sharedTest.diag" ) );
			sync::SnapshotSerializer::SerializeToDiagnosticFile(
				*privateSnapshot, vfs, file::VFS::kRoot.GetChild( "scratch", "snapshots", "privateTest.diag" ) );
		}
	}

	// Start sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.AddState( appstate::id::Gameplay_Overworld, DefaultCreator<appstate::Gameplay_Overworld>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Site, DefaultCreator<appstate::Gameplay_Site>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Battle, DefaultCreator<appstate::Gameplay_Battle>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Menus, DefaultCreator<appstate::Gameplay_Menus>::Create() );
	appStateMan.Start( appstate::GetFirstGameplayState() );
}



void Gameplay::OnExit( AppStateChangeContext& context )
{
	// Stop sub-states
	InternalState& internalState = *mInternalState;
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Stop();

	mInternalState = nullptr;

	// TODO: Multiplayer considerations and logic
	InputHelpers::ClearSinglePlayer();
}



void Gameplay::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;

	// Tick sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Tick( context.mCurrentTime, context.mElapsedTimeSinceLastTick );
	appStateMan.ApplyDeferredStateChange();
}

///////////////////////////////////////////////////////////////////////////////
}
