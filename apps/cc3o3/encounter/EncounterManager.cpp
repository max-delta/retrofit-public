#include "stdafx.h"
#include "EncounterManager.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/encounter/Encounter.h"
#include "cc3o3/state/components/Encounter.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/state/components/Combo.h"
#include "cc3o3/state/components/Vitality.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/StateLogging.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core_component/TypedObjectRef.h"
#include "core_state/VariableIdentifier.h"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

EncounterManager::EncounterManager() = default;



state::VariableIdentifier EncounterManager::FindEncounterIdentifier( input::PlayerID const& playerID ) const
{
	return state::VariableIdentifier( "encounter", rftl::to_string( playerID ) );
}



state::ObjectRef EncounterManager::FindEncounterObject( input::PlayerID const& playerID ) const
{
	return state::FindObjectByIdentifier( FindEncounterIdentifier( playerID ) );
}



state::MutableObjectRef EncounterManager::FindMutableEncounterObject( input::PlayerID const& playerID ) const
{
	return state::FindMutableObjectByIdentifier( FindEncounterIdentifier( playerID ) );
}



rftl::array<state::VariableIdentifier, kMaxSpawns> EncounterManager::FindSpawnIdentifiers( input::PlayerID const& playerID ) const
{
	rftl::array<state::VariableIdentifier, kMaxSpawns> retVal;

	// Find encounter object
	state::VariableIdentifier const encounterRoot = FindEncounterIdentifier( playerID );
	state::ObjectRef const encounter = FindEncounterObject( playerID );
	RFLOG_TEST_AND_FATAL( encounter.IsSet(), encounterRoot, RFCAT_CC3O3, "Failed to find encounter" );

	// For each spawn...
	state::VariableIdentifier const spawnRoot = encounterRoot.GetChild( "spawn" );
	for( size_t i_spawnIndex = 0; i_spawnIndex < kMaxSpawns; i_spawnIndex++ )
	{
		state::VariableIdentifier const charRoot = spawnRoot.GetChild( rftl::to_string( i_spawnIndex ) );

		retVal.at( i_spawnIndex ) = charRoot;
	}

	return retVal;
}



rftl::array<state::ObjectRef, kMaxSpawns> EncounterManager::FindSpawnObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::ObjectRef, kMaxSpawns> retVal;

	rftl::array<state::VariableIdentifier, kMaxSpawns> const identifiers = FindSpawnIdentifiers( playerID );
	for( size_t i_spawnIndex = 0; i_spawnIndex < kMaxSpawns; i_spawnIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_spawnIndex );
		state::ObjectRef const character = state::FindObjectByIdentifier( charRoot );
		retVal.at( i_spawnIndex ) = character;
	}

	return retVal;
}



rftl::array<state::MutableObjectRef, kMaxSpawns> EncounterManager::FindMutableSpawnObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::MutableObjectRef, kMaxSpawns> retVal;

	rftl::array<state::VariableIdentifier, kMaxSpawns> const identifiers = FindSpawnIdentifiers( playerID );
	for( size_t i_spawnIndex = 0; i_spawnIndex < kMaxSpawns; i_spawnIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_spawnIndex );
		state::MutableObjectRef const character = state::FindMutableObjectByIdentifier( charRoot );
		retVal.at( i_spawnIndex ) = character;
	}

	return retVal;
}



void EncounterManager::PrepareHackEnemyEncounter( EncounterID const& encounterID, input::PlayerID const& playerID ) const
{
	character::CharacterDatabase const& charDB = *gCharacterDatabase;

	// Get the encounter
	state::MutableObjectRef const encounterObject = FindMutableEncounterObject( playerID );
	state::comp::Encounter& encounter = *encounterObject.GetMutableComponentInstanceT<state::comp::Encounter>();

	// Get the available spawn objects
	rftl::array<state::MutableObjectRef, kMaxSpawns> const spawnObjects =
		FindMutableSpawnObjects( playerID );

	// Clear all
	for( size_t i_spawn = 0; i_spawn < kMaxSpawns; i_spawn++ )
	{
		encounter.mDeployed.at( i_spawn ) = false;
	}

	// HACK: Load here
	// TODO: Pre-load into manager
	file::VFSPath const encounterDescPath = paths::TablesRoot().GetChild( "world", "encounters", rftl::string( encounterID ) + ".oo" );
	Encounter const encounterSpec = Encounter::LoadFromDesc( encounterDescPath );
	RFLOG_TEST_AND_FATAL( encounterSpec.mEntities.size() <= kMaxSpawns, encounterDescPath, RFCAT_CC3O3, "Encounter has too many spawns" );

	// HACK: Spawn all
	// TODO: Support for reinforcements and delayed spawns
	size_t nextSpawnIndex = 0;
	for( Entity const& entity : encounterSpec.mEntities )
	{
		size_t const curSpawnIndex = nextSpawnIndex;
		nextSpawnIndex++;
		RF_ASSERT( curSpawnIndex < kMaxSpawns );

		encounter.mDeployed.at( curSpawnIndex ) = true;
		state::MutableObjectRef const spawn = spawnObjects.at( curSpawnIndex );

		// TODO: Figure this out
		combat::EntityClass const entityClass = combat::EntityClass::Player;

		// Combo
		state::comp::Combo& combo = *spawn.GetMutableComponentInstanceT<state::comp::Combo>();
		combo.mComboTarget = {};
		combo.mComboMeter = {};

		// Character
		state::comp::Character& chara = *spawn.GetMutableComponentInstanceT<state::comp::Character>();
		character::CharData& charData = chara.mCharData;
		charData = charDB.FetchExistingCharacter( entity.mCharacterID );
		character::Stats& stats = charData.mStats;

		// Vitality
		state::comp::Vitality& vitality = *spawn.GetMutableComponentInstanceT<state::comp::Vitality>();
		vitality.mCurHealth = gCombatEngine->LoCalcMaxHealth( stats.mMHealth, entityClass );
		vitality.mCurStamina = combat::kMaxStamina;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
