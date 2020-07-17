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

	using TeamIDs = rftl::static_vector<TeamIndex, kMaxTeamsPerFight>;
	using PartyIDs = rftl::static_vector<PartyID, kMaxTotalParties>;
	using CombatantIDs = rftl::static_vector<CombatantID, kMaxTotalCombatants>;


	//
	// Public methods
public:
	CombatInstance( WeakPtr<CombatEngine const> const& combatEngine );

	TeamIndex AddTeam();
	PartyID AddParty( TeamIndex teamID );
	CombatantID AddFighter( PartyID partyID );
	void RemoveFighter( CombatantID combatantID );

	TeamIDs GetTeamIDs() const;
	PartyIDs GetPartyIDs() const;
	CombatantIDs GetCombatantIDs() const;

	Team GetTeam( TeamIndex teamID ) const;
	Party GetParty( PartyID partyID ) const;
	Combatant GetCombatant( CombatantID combatantID ) const;

	void SetCombatant( CombatantID combatantID, Combatant const& combatant );
	void SetCombatant( CombatantID combatantID, component::MutableObjectRef const& character );

	rftl::array<element::InnateIdentifier, kFieldSize> GetFieldInfluence() const;
	void AddFieldInfluence( element::InnateIdentifier influence );
	void AddFieldInfluence( element::InnateIdentifier influence, size_t strength );
	void GenerateFieldInfluence( uint64_t seedHash );

	SimVal GetCounterGuage( PartyID party ) const;
	void IncreaseCounterGuage( PartyID party, SimVal value );

	AttackProfile PrepareAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength ) const;
	AttackResult ExecuteAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength );


	//
	// Private methods
private:
	Team const& GetTeamRef( TeamIndex teamID ) const;
	Team& GetMutableTeamRef( TeamIndex teamID );
	Party const& GetPartyRef( PartyID partyID ) const;
	Party& GetMutablePartyRef( PartyID partyID );
	Combatant const& GetCombatantRef( CombatantID combatantID ) const;
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
