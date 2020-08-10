#pragma once
#include "project.h"

#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/combat/Fighter.h"
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
		Fighter mFighter;
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

	using TeamIDs = rftl::static_vector<TeamID, kMaxTeamsPerFight>;
	using PartyIDs = rftl::static_vector<PartyID, kMaxTotalParties>;
	using FighterIDs = rftl::static_vector<FighterID, kMaxTotalFighters>;


	//
	// Public methods
public:
	CombatInstance( WeakPtr<CombatEngine const> const& combatEngine );

	TeamID AddTeam();
	PartyID AddParty( TeamID teamID );
	FighterID AddFighter( PartyID partyID );
	void RemoveFighter( FighterID fighterID );

	TeamIDs GetTeamIDs() const;
	PartyIDs GetPartyIDs() const;
	PartyIDs GetPartyIDs( TeamID teamID ) const;
	FighterIDs GetFighterIDs() const;
	FighterIDs GetFighterIDs( TeamID teamID ) const;
	FighterIDs GetFighterIDs( PartyID partyID ) const;

	TeamIDs GetOpposingTeams( TeamID teamID ) const;

	Team GetTeam( TeamID teamID ) const;
	Party GetParty( PartyID partyID ) const;
	Fighter GetFighter( FighterID fighterID ) const;

	void SetCombatant( FighterID fighterID, Fighter const& combatant );
	void SetCombatant( FighterID fighterID, component::MutableObjectRef const& character );

	rftl::array<element::InnateIdentifier, kFieldSize> GetFieldInfluence() const;
	void AddFieldInfluence( element::InnateIdentifier influence );
	void AddFieldInfluence( element::InnateIdentifier influence, size_t strength );
	void GenerateFieldInfluence( uint64_t seedHash );

	SimVal GetCounterGuage( PartyID party ) const;
	void IncreaseCounterGuage( PartyID party, SimVal value );

	FighterIDs GetValidAttackTargets( FighterID attackerID ) const;
	AttackProfile PrepareAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const;
	AttackResult ExecuteAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength );


	//
	// Private methods
private:
	Team const& GetTeamRef( TeamID teamID ) const;
	Team& GetMutableTeamRef( TeamID teamID );
	Party const& GetPartyRef( PartyID partyID ) const;
	Party& GetMutablePartyRef( PartyID partyID );
	Fighter const& GetFighterRef( FighterID fighterID ) const;
	Fighter& GetMutableFighterRef( FighterID fighterID );


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
