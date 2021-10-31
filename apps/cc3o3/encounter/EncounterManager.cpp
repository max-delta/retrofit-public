#include "stdafx.h"
#include "EncounterManager.h"

#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/StateLogging.h"

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

///////////////////////////////////////////////////////////////////////////////
}
