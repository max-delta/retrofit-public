#include "stdafx.h"
#include "Gameplay.h"

#include "cc3o3/appstates/gameplay/Gameplay_Overworld.h"
#include "cc3o3/appstates/gameplay/Gameplay_Site.h"
#include "cc3o3/appstates/gameplay/Gameplay_Battle.h"
#include "cc3o3/appstates/gameplay/Gameplay_Menus.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/Company.h"
#include "cc3o3/state/objects/OverworldCharacter.h"
#include "cc3o3/state/objects/SiteCharacter.h"
#include "cc3o3/state/objects/BattleCharacter.h"
#include "cc3o3/state/components/Roster.h"
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


namespace RF { namespace cc { namespace appstate {
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
			character::Character character = charDB.FetchExistingCharacter( characterID );
			charValidate.SanitizeForGameplay( character );
			charDB.OverwriteExistingCharacter( characterID, std::move( character ) );
		}
	}

	// Composite characters
	{
		sprite::CharacterCreator& charCreate = *gCharacterCreator;

		sprite::CompositeCharacterParams params = {};
		params.mMode = "24";
		params.mCompositeWidth = 24;
		params.mCompositeHeight = 24;
		params.mCharPiecesDir = paths::CharacterPieces();

		for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
		{
			character::Character const input = charDB.FetchExistingCharacter( characterID );

			params.mBaseId = input.mVisuals.mBase;
			params.mTopId = input.mVisuals.mTop;
			params.mBottomId = input.mVisuals.mBottom;
			params.mHairId = input.mVisuals.mHair;
			params.mSpeciesId = input.mVisuals.mSpecies;
			params.mOutputDir = paths::CompositeCharacters().GetChild( characterID );

			sprite::CompositeCharacter output = charCreate.CreateCompositeCharacter( params );
			charDB.SubmitOrOverwriteComposite( characterID, rftl::move( output ) );
		}
	}

	// Set up objects
	{
		using namespace state;
		using namespace state::obj;

		rollback::RollbackManager& rollMan = *app::gRollbackManager;
		rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
		rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();
		rollback::Window& sharedWindow = sharedDomain.GetMutableWindow();
		rollback::Window& privateWindow = privateDomain.GetMutableWindow();

		// Set up each company
		// TODO: Multiple companies for competitive multiplayer
		{
			// HACK: Only local player
			input::PlayerID const playerID = input::HardcodedGetLocalPlayer();

			rftl::string const playerIDAsString = ( rftl::stringstream() << math::integer_cast<size_t>( playerID ) ).str();
			VariableIdentifier const companyRoot( "company", playerIDAsString );

			// Create company object
			MutableObjectRef const company = CreateCompany( sharedWindow, privateWindow, companyRoot );
			comp::Roster& roster = *company.GetMutableComponentInstanceT<comp::Roster>();

			// Set up all characters
			// NOTE: Characters with the company as their root are implicitly
			//  on the company's roster
			// TODO: Data-driven list
			comp::Roster::RosterIndex rosterIndex = comp::Roster::kInititialRosterIndex;
			VariableIdentifier const charRoot = companyRoot.GetChild( "member" );
			for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
			{
				rftl::string const rosterIndexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( rosterIndex ) ).str();

				MutableObjectRef const newChar = CreateOverworldCharacterFromDB(
					sharedWindow, privateWindow,
					charRoot.GetChild( rosterIndexAsString ), characterID );
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
}}}
