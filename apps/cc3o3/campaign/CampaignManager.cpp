#include "stdafx.h"
#include "CampaignManager.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/AppStatesFwd.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/campaign/Campaign.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/combat/FightController.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/overworld/Overworld.h"
#include "cc3o3/save/SaveBlob.h"
#include "cc3o3/site/Site.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/Progression.h"
#include "cc3o3/state/objects/Company.h"
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

static constexpr char const* kHardcodedCharacterIDs[] = { "claire", "brick", "laura" };
static constexpr size_t kHardcodedRosterCount = 3;



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



combat::TeamID BuildHackEnemyTeam( combat::CombatInstance& setup )
{
	using namespace combat;

	TeamID const team = setup.AddTeam();
	PartyID const party = setup.AddParty( team );

	rftl::array<Fighter, 2> enemies = {};
	{
		Fighter& enemy = enemies.at( 0 );
		enemy.mInnate = element::MakeInnateIdentifier( element::InnateString{ 'r', 'e', 'd' } );
		enemy.mMaxHealth = 999;
		enemy.mCurHealth = enemy.mMaxHealth;
		enemy.mMaxStamina = 7;
		enemy.mCurStamina = enemy.mMaxStamina;
		enemy.mPhysAtk = 2;
		enemy.mPhysDef = 2;
		enemy.mElemAtk = 2;
		enemy.mElemDef = 2;
		enemy.mBalance = 2;
		enemy.mTechniq = 2;
	}
	{
		Fighter& enemy = enemies.at( 1 );
		enemy.mInnate = element::MakeInnateIdentifier( element::InnateString{ 'r', 'e', 'd' } );
		enemy.mMaxHealth = 999;
		enemy.mCurHealth = enemy.mMaxHealth;
		enemy.mMaxStamina = 7;
		enemy.mCurStamina = enemy.mMaxStamina;
		enemy.mPhysAtk = 2;
		enemy.mPhysDef = 2;
		enemy.mElemAtk = 2;
		enemy.mElemDef = 2;
		enemy.mBalance = 2;
		enemy.mTechniq = 2;
	}
	for( Fighter const& enemy : enemies )
	{
		FighterID const enemyID = setup.AddFighter( party );
		setup.SetCombatant( enemyID, enemy );
	}

	return team;
}

}
///////////////////////////////////////////////////////////////////////////////

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
}



void CampaignManager::HardcodedSinglePlayerObjectSetup()
{
	using namespace state;
	using namespace state::obj;

	VerifyCampaignLoaded();

	rollback::RollbackManager& rollMan = *gRollbackManager;
	rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
	rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();
	rollback::Window& sharedWindow = sharedDomain.GetMutableWindow();
	rollback::Window& privateWindow = privateDomain.GetMutableWindow();

	// Set up each company
	// TODO: Multiple companies for competitive multiplayer
	{
		// HACK: Only local player
		input::PlayerID const playerID = appstate::InputHelpers::GetSinglePlayer();
		VariableIdentifier const companyRoot( "company", rftl::to_string( playerID ) );

		// Create company object
		CreateCompany( sharedWindow, privateWindow, companyRoot );

		// Set up characters
		// NOTE: Characters with the company as their root are implicitly
		//  on the company's roster
		// TODO: Data-driven list
		company::RosterIndex rosterIndex = company::kInititialRosterIndex;
		VariableIdentifier const charRoot = companyRoot.GetChild( "member" );
		for( char const* const& characterID : details::kHardcodedCharacterIDs )
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

			rosterIndex++;
		}
	}
}



void CampaignManager::HardcodedSinglePlayerApplyProgression()
{
	using namespace state;
	using namespace state::obj;

	VerifyCampaignLoaded();

	// Configure each company
	// TODO: Multiple companies for competitive multiplayer
	{
		// HACK: Only local player
		input::PlayerID const playerID = appstate::InputHelpers::GetSinglePlayer();
		VariableIdentifier const companyRoot( "company", rftl::to_string( playerID ) );

		// Get company object
		MutableObjectRef const company = FindMutableObjectByIdentifier( companyRoot );
		comp::Roster& roster = *company.GetMutableComponentInstanceT<comp::Roster>();
		comp::Progression& progression = *company.GetMutableComponentInstanceT<comp::Progression>();

		// HACK: All valid characters are eligible
		// TODO: Tie to story progression
		for( size_t i_char = 0; i_char < details::kHardcodedRosterCount; i_char++ )
		{
			company::RosterIndex const rosterIndex =
				math::integer_cast<company::RosterIndex>(
					company::kInititialRosterIndex + i_char );
			roster.mEligible.at( rosterIndex ) = true;
		}

		// HACK: Make sure some hard-coded characters are present
		// TODO: Check active team conditions, auto-fill if needed
		RF_ASSERT( roster.mEligible.at( 0 ) );
		RF_ASSERT( roster.mEligible.at( 1 ) );
		RF_ASSERT( roster.mEligible.at( 2 ) );
		roster.mActiveTeam.at( 0 ) = 0;
		roster.mActiveTeam.at( 1 ) = 2;
		roster.mActiveTeam.at( 2 ) = 1;

		// HACK: Set some fake progression
		progression.mStoryTier = 6;
	}
}



overworld::Overworld CampaignManager::LoadDataForOverworld()
{
	VerifyCampaignLoaded();

	// Load map
	// HACK: Hard-coded
	// TODO: Figure out from current pawn position
	static constexpr char const kHackMap[] = "island1";
	file::VFSPath const mapDescPath = paths::TablesRoot().GetChild( "world", "overworlds", rftl::string( kHackMap ) + ".oo" );
	return overworld::Overworld::LoadFromDesc( mapDescPath );
}



std::string CampaignManager::DetermineOverworldAreaLocKey( rftl::string identifier )
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
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Site );

	return true;
}



site::Site CampaignManager::LoadDataForSite()
{
	VerifyCampaignLoaded();

	// Load site
	// HACK: Hard-coded
	// TODO: Figure out from current pawn position
	static constexpr char const kHackSite[] = "temp";
	file::VFSPath const siteDescPath = paths::TablesRoot().GetChild( "world", "sites", rftl::string( kHackSite ) + ".oo" );
	return site::Site::LoadFromDesc( siteDescPath );
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
		details::BuildHackEnemyTeam( setup );
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
