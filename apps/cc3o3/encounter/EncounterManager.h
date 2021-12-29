#pragma once
#include "cc3o3/encounter/EncounterFwd.h"

#include "cc3o3/state/StateFwd.h"

#include "GameInput/InputFwd.h"

#include "core/macros.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/array"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

class EncounterManager
{
	RF_NO_COPY( EncounterManager );

	//
	// Types
public:
	using EncounterID = rftl::immutable_string;


	//
	// Public methods
public:
	EncounterManager();

	// Locate various encounter-related objects
	state::VariableIdentifier FindEncounterIdentifier( input::PlayerID const& playerID ) const;
	state::ObjectRef FindEncounterObject( input::PlayerID const& playerID ) const;
	state::MutableObjectRef FindMutableEncounterObject( input::PlayerID const& playerID ) const;
	rftl::array<state::VariableIdentifier, kMaxSpawns> FindSpawnIdentifiers( input::PlayerID const& playerID ) const;
	rftl::array<state::ObjectRef, kMaxSpawns> FindSpawnObjects( input::PlayerID const& playerID ) const;
	rftl::array<state::MutableObjectRef, kMaxSpawns> FindMutableSpawnObjects( input::PlayerID const& playerID ) const;

	void PrepareHackEnemyEncounter( EncounterID const& encounterID, input::PlayerID const& playerID ) const;


	//
	// Private data
private:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}
