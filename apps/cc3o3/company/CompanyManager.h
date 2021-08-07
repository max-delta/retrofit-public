#pragma once
#include "cc3o3/company/CompanyFwd.h"

#include "cc3o3/elements/ElementFwd.h"
#include "cc3o3/char/CharFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameInput/InputFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/unordered_map"
#include "rftl/array"


// Forwards
namespace RF::cc::state::comp {
class Loadout;
class Progression;
}
namespace RF::cc::element {
class ElementDatabase;
}

namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

class CompanyManager
{
	RF_NO_COPY( CompanyManager );

	//
	// Types and constants
public:
	using ElementCounts = rftl::unordered_map<element::ElementIdentifier, size_t>;


	//
	// Public methods
public:
	CompanyManager(
		WeakPtr<file::VFS const> const& vfs,
		WeakPtr<element::ElementDatabase const> const& elementDatabase );

	// Locate various company-related objects
	state::VariableIdentifier FindCompanyIdentifier( input::PlayerID const& playerID ) const;
	state::ObjectRef FindCompanyObject( input::PlayerID const& playerID ) const;
	rftl::array<state::VariableIdentifier, kRosterSize> FindRosterIdentifiers( input::PlayerID const& playerID ) const;
	rftl::array<state::ObjectRef, kRosterSize> FindRosterObjects( input::PlayerID const& playerID ) const;
	rftl::array<state::MutableObjectRef, kRosterSize> FindMutableRosterObjects( input::PlayerID const& playerID ) const;
	rftl::array<state::VariableIdentifier, kActiveTeamSize> FindActivePartyIdentifiers( input::PlayerID const& playerID ) const;
	rftl::array<state::ObjectRef, kActiveTeamSize> FindActivePartyObjects( input::PlayerID const& playerID ) const;
	rftl::array<state::MutableObjectRef, kActiveTeamSize> FindMutableActivePartyObjects( input::PlayerID const& playerID ) const;
	state::VariableIdentifier FindActivePartyCharacterIdentifier( input::PlayerID const& playerID, size_t partyIndex ) const;
	state::ObjectRef FindActivePartyCharacterObject( input::PlayerID const& playerID, size_t partyIndex ) const;
	state::MutableObjectRef FindMutableActivePartyCharacterObject( input::PlayerID const& playerID, size_t partyIndex ) const;

	// The company's total available elements are determined by progression
	ElementCounts CalcTotalElements( state::ObjectRef const& company ) const;
	ElementCounts CalcTotalElements( state::comp::Progression const& progression ) const;

	// Elements are assigned without validation (performed seperately)
	void AssignElementToCharacter( state::MutableObjectRef character, character::ElementSlotIndex slot, element::ElementIdentifier element );
	void AssignElementToCharacter( state::comp::Loadout& loadout, character::ElementSlotIndex slot, element::ElementIdentifier element );

	// Save/load loadouts to/from disk
	void ReadLoadoutsFromSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID );
	void WriteLoadoutsToSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID );

	// TODO: Need various validation machinery:
	//  * Loadout setup / UI helpers
	//  * Hard-fail gameplay verification
	//  * Cheat detection
	void TODO_ValidateLoadouts();


	//
	// Private data
private:
	WeakPtr<file::VFS const> const mVfs;
	WeakPtr<element::ElementDatabase const> const mElementDatabase;
};

///////////////////////////////////////////////////////////////////////////////
}
