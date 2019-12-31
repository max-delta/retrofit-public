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
#include "cc3o3/state/objects/OverworldCharacter.h"
#include "cc3o3/state/objects/SiteCharacter.h"
#include "cc3o3/state/objects/BattleCharacter.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameAppState/AppStateTickContext.h"
#include "GameSprite/CharacterCreator.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFSPath.h"
#include "Rollback/RollbackManager.h"

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
		params.mCompositeWidth = 32;
		params.mCompositeHeight = 32;
		params.mCharPiecesDir = paths::CharacterPieces();

		for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
		{
			character::Character const input = charDB.FetchExistingCharacter( characterID );

			params.mBaseId = input.mVisuals.mBase;
			params.mClothingId = input.mVisuals.mClothing;
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
		rollback::Window& sharedWindow = rollMan.GetMutableSharedDomain().GetMutableWindow();
		rollback::Window& privateWindow = rollMan.GetMutablePrivateDomain().GetMutableWindow();

		// Set up each company
		// TODO: Multiple companies for competitive multiplayer
		{
			// HACK: Only local player
			input::PlayerID const playerID = input::HardcodedGetLocalPlayer();

			char const playerIDAsChar = math::integer_cast<char>( '0' + playerID );
			VariableIdentifier const companyRoot{ rftl::string( "company" ) + playerIDAsChar };

			// TODO: Create company object

			// TODO: Create roster, and store on company object

			// Set up all characters
			// TODO: Data-driven list
			size_t rosterIndex = 1;
			VariableIdentifier const charRoot = companyRoot.GetChild( "char" );
			for( character::CharacterDatabase::CharacterID const& characterID : characterIDs )
			{
				rftl::immutable_string const rosterIndexAsString = ( rftl::stringstream() << rosterIndex ).str();
				rosterIndex++;

				MutableObjectRef const newChar = CreateOverworldCharacterFromDB(
					sharedWindow, privateWindow,
					charRoot.GetChild( rosterIndexAsString ), characterID );
				MakeSiteCharacterFromDB(
					sharedWindow, privateWindow,
					newChar, characterID );
				MakeBattleCharacterFromDB(
					sharedWindow, privateWindow,
					newChar, characterID );

				// TODO: Add to company roster
			}

			// TODO: Check active team conditions, auto-fill if needed
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
