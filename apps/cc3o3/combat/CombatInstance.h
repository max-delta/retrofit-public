#pragma once
#include "project.h"

#include "cc3o3/combat/Combatant.h"
#include "cc3o3/combat/Party.h"
#include "cc3o3/combat/Team.h"
#include "cc3o3/combat/Field.h"
#include "cc3o3/elements/ElementFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "core_component/ObjectRef.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/extension/static_vector.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

class CombatInstance
{
	RF_DEFAULT_COPY( CombatInstance );

	//
	// Types and constants
public:
	struct FighterEntry
	{
		Combatant mCombatant;
		component::MutableObjectRef mPersist;
	};
	using Fighters = rftl::static_vector<FighterEntry, kMaxFightersPerParty>;

	struct PartyEntry
	{
		Party mParty;
		Fighters mFighters;
	};
	using Parties = rftl::static_vector<PartyEntry, kMaxPartiesPerTeam>;

	struct TeamEntry
	{
		Team mTeam;
		Parties mParties;
	};
	using Teams = rftl::static_vector<TeamEntry, kMaxTeamsPerFight>;


	//
	// Public methods
public:
	CombatInstance( WeakPtr<CombatEngine const> const& combatEngine );

	TeamIndex AddTeam();
	PartyID AddParty( TeamIndex teamID );
	CombatantID AddFighter( PartyID partyID );
	void RemoveFighter( CombatantID combatantID );

	Combatant const& GetCombatantRef( CombatantID combatantID ) const;
	void SetCombatant( CombatantID combatantID, Combatant const& combatant );
	void SetCombatant( CombatantID combatantID, component::MutableObjectRef const& character );

	rftl::array<element::InnateIdentifier, kFieldSize> GetFieldInfluence() const;
	void AddFieldInfluence( element::InnateIdentifier influence );
	void AddFieldInfluence( element::InnateIdentifier influence, size_t strength );

	SimVal GetCounterGuage( PartyID party ) const;
	void IncreaseCounterGuage( PartyID party, SimVal value );

	AttackProfile PrepareAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength ) const;
	AttackResult ExecuteAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength );


	//
	// Private methods
private:
	Combatant& GetMutableCombatantRef( CombatantID combatantID );


	//
	// Private data
private:
	WeakPtr<CombatEngine const> mCombatEngine;

	Teams mTeams;
	Field mField;
};

///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr size_t kCombatInstanceSize = sizeof( CombatInstance );
static_assert( kCombatInstanceSize < 2048, "Combat instance is growing large, re-evaluate" );

}
///////////////////////////////////////////////////////////////////////////////
}
