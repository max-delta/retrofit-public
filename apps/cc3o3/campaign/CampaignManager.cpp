#include "stdafx.h"
#include "CampaignManager.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/campaign/Campaign.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/combat/FightController.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/encounter/EncounterManager.h"
#include "cc3o3/overworld/Overworld.h"
#include "cc3o3/save/SaveBlob.h"
#include "cc3o3/site/Site.h"
#include "cc3o3/state/components/Encounter.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/Progression.h"
#include "cc3o3/state/objects/Company.h"
#include "cc3o3/state/objects/Encounter.h"
#include "cc3o3/state/objects/BaseCharacter.h"
#include "cc3o3/state/objects/OverworldCharacter.h"
#include "cc3o3/state/objects/SiteCharacter.h"
#include "cc3o3/state/objects/BattleCharacter.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/StateHelpers.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"
#include "GameSprite/CharacterCreator.h"

#include "Rollback/RollbackManager.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////
namespace details {

combat::PartyID BuildPlayerParty( combat::CombatInstance& setup, combat::TeamID team, input::PlayerID playerID )
{
	using namespace combat;

	PartyID const party = setup.AddParty( team );

	// Get the active party characters
	rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
		gCompanyManager->FindMutableActivePartyObjects( playerID );

	// For each active team member...
	for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
	{
		state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
		if( character.IsSet() == false )
		{
			// Not active
			continue;
		}

		// Add to party
		FighterID const fighter = setup.AddFighter( party );
		setup.SetCombatant( fighter, character );
	}

	return party;
}



combat::TeamID BuildHackEnemyTeam( combat::CombatInstance& setup, input::PlayerID hostPlayerID, rftl::string identifier )
{
	using namespace combat;

	encounter::EncounterManager const& encounterManager = *gEncounterManager;

	TeamID const team = setup.AddTeam();
	PartyID const party = setup.AddParty( team );

	// Prepare an encounter
	encounterManager.PrepareHackEnemyEncounter( identifier, hostPlayerID );

	// Get the encounter
	state::MutableObjectRef const encounterObject = encounterManager.FindMutableEncounterObject( hostPlayerID );
	state::comp::Encounter& encounter = *encounterObject.GetMutableComponentInstanceT<state::comp::Encounter>();

	// Get the available spawn objects
	rftl::array<state::MutableObjectRef, encounter::kMaxSpawns> const spawnObjects =
		encounterManager.FindMutableSpawnObjects( hostPlayerID );

	// For each spawn...
	for( size_t i_spawn = 0; i_spawn < encounter::kMaxSpawns; i_spawn++ )
	{
		if( encounter.mDeployed.at( i_spawn ) == false )
		{
			// Not active
			continue;
		}

		// NOTE: Spawn objects should always exist as scratch, even if not used
		state::MutableObjectRef const& spawn = spawnObjects.at( i_spawn );
		RF_ASSERT( spawn.IsSet() );

		// Add to party
		FighterID const fighter = setup.AddFighter( party );
		setup.SetCombatant( fighter, spawn );
	}

	return team;
}

}
///////////////////////////////////////////////////////////////////////////////

CampaignManager::CampaignManager()
{
	RF_TODO_ANNOTATION( "Remove campaign fallbacks in retail builds" );
	mLastOverworldTransition = appstate::GetFallbackOverworldIdentifier();
	mLastSiteTransition = appstate::GetFallbackSiteIdentifier();
	mLastEncounterTransition = appstate::GetFallbackEncounterIdentifier();
}



bool CampaignManager::PrepareCampaign( save::SaveBlob const& saveBlob )
{
	mCampaignName = saveBlob.mFile.mCampaignName;
	if( mCampaignName.empty() )
	{
		RFLOG_ERROR( nullptr, RFCAT_CC3O3, "Save blob has empty campaign name" );
		return false;
	}

	// Load campaign
	file::VFSPath const mapDescPath = paths::Campaigns().GetChild( mCampaignName );
	mCampaign = Campaign::LoadFromFolder( mapDescPath );
	if( mCampaign == nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_CC3O3, "Failed to load campaign from folder" );
		return false;
	}

	return true;
}



bool CampaignManager::LoadCampaignProgress( save::SaveBlob const& saveBlob )
{
	VerifyCampaignLoaded();

	// TODO: Actually load stuff
	TODO_ChangeOverworldLocation( 0 );

	// TODO: Store read-only temporary for later initialization
	return true;
}



bool CampaignManager::SaveCampaignProgress( save::SaveBlob& saveBlob )
{
	VerifyCampaignLoaded();

	saveBlob.mFile.mCampaignName = mCampaignName;

	// TODO: Crawl data and write to save
	return true;
}



void CampaignManager::PrepareFastTrackOverworld()
{
	RF_TODO_ANNOTATION( "More FastTrack control knobs" );
}



void CampaignManager::PrepareFastTrackSite()
{
	RF_TODO_ANNOTATION( "More FastTrack control knobs" );
}



void CampaignManager::PrepareFastTrackMenus()
{
	RF_TODO_ANNOTATION( "More FastTrack control knobs" );
}



void CampaignManager::PrepareFastTrackBattle()
{
	RF_TODO_ANNOTATION( "More FastTrack control knobs" );
}



void CampaignManager::PrepareFastTrackCutscene()
{
	RF_TODO_ANNOTATION( "More FastTrack control knobs" );
}



appstate::AppStateID CampaignManager::PrepareInitialGameplayState()
{
	// HACK: Always overworld
	RF_TODO_ANNOTATION( "Set initial game state based on save file" );
	return appstate::id::Gameplay_Overworld;
}



void CampaignManager::HardcodedSinglePlayerCharacterLoad()
{
	VerifyCampaignLoaded();

	// Prepare the character database
	character::CharacterDatabase& charDB = *gCharacterDatabase;
	{
		// Reset
		charDB.DeleteAllCharacters();

		// Load base characters
		file::VFSPath const base = paths::BaseCharacters();
		size_t const charsLoaded = charDB.LoadFromPersistentStorage( base );
		RFLOGF_INFO( base, RFCAT_CC3O3, "Loaded %llu chars", charsLoaded );
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

			if( input.mVisuals.mCompositable == false )
			{
				continue;
			}
			else
			{
				// TODO: There needs to be a mechanism for non-composited
				//  characters
				RFLOG_WARNING( nullptr, RFCAT_CC3O3, "TODO: Handle uncompositable characters" );
			}

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
}



void CampaignManager::HardcodedSinglePlayerObjectSetup()
{
	using namespace state;
	using namespace state::obj;

	VerifyCampaignLoaded();
	Campaign const& campaign = *mCampaign;

	rollback::RollbackManager& rollMan = *gRollbackManager;
	rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
	rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();
	rollback::Window& sharedWindow = sharedDomain.GetMutableWindow();
	rollback::Window& privateWindow = privateDomain.GetMutableWindow();

	// Set up each company
	// TODO: Multiple companies for competitive multiplayer
	{
		company::CompanyManager const& companyManager = *gCompanyManager;

		// HACK: Only local player
		input::PlayerID const playerID = appstate::InputHelpers::GetSinglePlayer();
		VariableIdentifier const companyRoot = companyManager.FindCompanyIdentifier( playerID );
		Company const& companyDesc = campaign.mPlayerCompany;

		// Create company object
		CreateCompany( sharedWindow, privateWindow, companyRoot );

		// Set up characters
		// NOTE: Characters with the company as their root are implicitly
		//  on the company's roster
		company::RosterIndex rosterIndex = company::kInititialRosterIndex;
		VariableIdentifier const charRoot = companyRoot.GetChild( "member" );
		for( RosterMember const& member : companyDesc.mRoster )
		{
			rftl::string const& characterID = member.mIdentifier;
			MutableObjectRef const newChar = CreateBaseCharacterFromDB(
				sharedWindow, privateWindow,
				charRoot.GetChild( rftl::to_string( rosterIndex ) ), characterID );
			MakeOverworldCharacterFromDB(
				sharedWindow, privateWindow,
				newChar, characterID );
			MakeSiteCharacterFromDB(
				sharedWindow, privateWindow,
				newChar, characterID );
			MakeBattleCharacter(
				sharedWindow, privateWindow,
				newChar );

			rosterIndex++;
		}
	}

	// Set up encounter scratch space
	{
		encounter::EncounterManager const& encounterManager = *gEncounterManager;

		// HACK: Only local player
		// NOTE: The idea is that maybe there is a situation where there could
		//  be parallel battles
		input::PlayerID const playerID = appstate::InputHelpers::GetSinglePlayer();
		VariableIdentifier const encounterRoot = encounterManager.FindEncounterIdentifier( playerID );

		// Create encounter object
		CreateEncounter( sharedWindow, privateWindow, encounterRoot );

		// Set up spawns
		// NOTE: Spawns with the encounter as their root are implicitly
		//  part of the encounter
		encounter::SpawnIndex spawnIndex = encounter::kInititialSpawnIndex;
		VariableIdentifier const charRoot = encounterRoot.GetChild( "spawn" );
		for( size_t i_spawnIndex = 0; i_spawnIndex < encounter::kMaxSpawns; i_spawnIndex++ )
		{
			character::CharData const invalidCharData = {};
			MutableObjectRef const newChar = CreateBaseCharacter(
				sharedWindow, privateWindow,
				charRoot.GetChild( rftl::to_string( spawnIndex ) ), invalidCharData );
			MakeBattleCharacter(
				sharedWindow, privateWindow,
				newChar );

			spawnIndex++;
		}
	}
}



void CampaignManager::HardcodedSinglePlayerApplyProgression()
{
	using namespace state;
	using namespace state::obj;

	VerifyCampaignLoaded();
	Campaign const& campaign = *mCampaign;

	// Configure each company
	// TODO: Multiple companies for competitive multiplayer
	{
		// HACK: Only local player
		input::PlayerID const playerID = appstate::InputHelpers::GetSinglePlayer();
		VariableIdentifier const companyRoot( "company", rftl::to_string( playerID ) );
		Company const& companyDesc = campaign.mPlayerCompany;

		// Get company object
		MutableObjectRef const company = FindMutableObjectByIdentifier( companyRoot );
		comp::Roster& roster = *company.GetMutableComponentInstanceT<comp::Roster>();
		comp::Progression& progression = *company.GetMutableComponentInstanceT<comp::Progression>();

		// HACK: All valid characters are eligible
		// TODO: Tie to story progression
		for( size_t i_char = 0; i_char < companyDesc.mRoster.size(); i_char++ )
		{
			company::RosterIndex const rosterIndex =
				math::integer_cast<company::RosterIndex>(
					company::kInititialRosterIndex + i_char );
			roster.mEligible.at( rosterIndex ) = true;
		}

		// HACK: Make sure some hard-coded characters are present
		// TODO: Check active team conditions, auto-fill if needed
		RF_ASSERT( companyDesc.mRoster.size() == 3 );
		RF_ASSERT( roster.mEligible.at( 0 ) );
		RF_ASSERT( roster.mEligible.at( 1 ) );
		RF_ASSERT( roster.mEligible.at( 2 ) );
		roster.mActiveTeam.at( 0 ) = 1;
		roster.mActiveTeam.at( 1 ) = 2;
		roster.mActiveTeam.at( 2 ) = 0;

		// HACK: Set some fake progression
		progression.mStoryTier = 6;

		// Apply the effects of progression
		gCompanyManager->UpdateRosterGridMasks( playerID );
	}
}



void CampaignManager::EnterMenus( appstate::AppStateTickContext& context )
{
	// HACK: Always enter, never check anything
	// TODO: Restrict this?
	RF_TODO_ANNOTATION( "Validation? Setup?" );

	// Bring up menus
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Menus );
}



void CampaignManager::LeaveMenus( appstate::AppStateTickContext& context )
{
	// HACK: Always assume returning to overworld
	// TODO: Proper logic
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Overworld );
}



void CampaignManager::TODO_ChangeOverworldLocation( int todo )
{
	// TODO: Setup which overworld to be in, and where in it to be
	( (void)mLastOverworldTransition );
}



void CampaignManager::TravelToOverworld( appstate::AppStateTickContext& context )
{
	VerifyCampaignLoaded();

	// HACK: Just pop into overworld
	// TODO: Validation? Setup?
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Overworld );
}



overworld::Overworld CampaignManager::LoadDataForOverworld()
{
	VerifyCampaignLoaded();

	// Load map
	file::VFSPath const mapDescPath = paths::TablesRoot().GetChild( "world", "overworlds", rftl::string( mLastOverworldTransition ) + ".oo" );
	return overworld::Overworld::LoadFromDesc( mapDescPath );
}



rftl::string CampaignManager::DetermineOverworldAreaLocKey( rftl::string identifier )
{
	VerifyCampaignLoaded();

	// TODO: Under some conditions, this may change, such as to a key for "???"
	return rftl::string( "$location_area_" ) + identifier;
}



bool CampaignManager::TryInteractWithOverworldArea( appstate::AppStateTickContext& context, rftl::string identifier )
{
	VerifyCampaignLoaded();

	// HACK: Just pop into site
	// TODO: Make sure party can actually enter site, and set it up
	// TODO: Set site exit position, and also ensure which overworld it's on
	//  (otherwise speedrunners will glitch it out somehow to teleport around)
	TODO_ChangeOverworldLocation( 0 );
	( (void)mLastSiteTransition );
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Site );

	return true;
}



site::Site CampaignManager::LoadDataForSite()
{
	VerifyCampaignLoaded();

	// Load site
	file::VFSPath const siteDescPath = paths::TablesRoot().GetChild( "world", "sites", rftl::string( mLastSiteTransition ) + ".oo" );
	return site::Site::LoadFromDesc( siteDescPath );
}



bool CampaignManager::CanLeaveSite()
{
	// HACK: Always allow
	RF_TODO_ANNOTATION(
		"Check if player can leave site, notify why not if restricted" );
	return true;
}



void CampaignManager::LeaveSite( appstate::AppStateTickContext& context )
{
	// HACK: Just travel to the overworld
	RF_TODO_ANNOTATION(
		"Ask campaign what to return to, which"
		" there might not be anything allowed"
		" to return to at certain plot points,"
		" which would need to block leaving"
		" and notify the player why they can't"
		" leave yet" );
	TravelToOverworld( context );
}



void CampaignManager::StartEncounter( appstate::AppStateTickContext& context, rftl::string identifier )
{
	VerifyCampaignLoaded();

	// HACK: Just pop into battle
	// TODO: Actually set it up
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Battle );
}



void CampaignManager::HardcodedCombatSetup( combat::FightController& fight )
{
	using namespace combat;

	VerifyCampaignLoaded();

	UniquePtr<CombatInstance> instance = DefaultCreator<CombatInstance>::Create( gCombatEngine );
	CombatInstance& setup = *instance;

	input::PlayerID const singlePlayerHack = appstate::InputHelpers::GetSinglePlayer();

	PartyID localPartyID = {};

	// Setup players
	// HACK: One player
	// TODO: Multiplayer, opposing teams
	// TODO: Multiplayer, same team
	rftl::vector<input::PlayerID> const playerIDs = { singlePlayerHack };
	for( input::PlayerID const& playerID : playerIDs )
	{
		TeamID const playerTeam = setup.AddTeam();
		PartyID const playerParty = details::BuildPlayerParty( setup, playerTeam, playerID );
		if( playerID == singlePlayerHack )
		{
			localPartyID = playerParty;
		}
	}

	// Setup NPCs
	// HACK: Hard-coded
	// TODO: Encounters
	if constexpr( true )
	{
		details::BuildHackEnemyTeam( setup, singlePlayerHack, mLastEncounterTransition );
	}

	// Setup initial field influence
	// HACK: Hard-coded
	// TODO: Encounter specified override or multiplayer-minded hash source
	setup.GenerateFieldInfluence( 0 );

	fight.SetupFromCombatInstance( rftl::move( instance ), localPartyID );
}

///////////////////////////////////////////////////////////////////////////////

void CampaignManager::VerifyCampaignLoaded()
{
	RF_ASSERT( mCampaignName.empty() == false );
	RF_ASSERT( mCampaign != nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}
