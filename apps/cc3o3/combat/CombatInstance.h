#pragma once
#include "project.h"

#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/combat/Fighter.h"
#include "cc3o3/combat/Party.h"
#include "cc3o3/combat/Team.h"
#include "cc3o3/combat/Field.h"
#include "cc3o3/elements/ElementFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "core_component/TypedObjectRef.h"

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
		state::MutableObjectRef mPersist;
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
	void SetCombatant( FighterID fighterID, state::MutableObjectRef const& character );

	void CommitCombatData() const;
	void ReloadCombatData();

	rftl::array<element::InnateIdentifier, kFieldSize> GetFieldInfluence() const;
	void AddFieldInfluence( element::InnateIdentifier influence );
	void AddFieldInfluence( element::InnateIdentifier influence, size_t strength );
	void GenerateFieldInfluence( uint64_t seedHash );

	SimVal GetCounterGuage( PartyID party ) const;
	void IncreaseCounterGuage( PartyID party, SimVal value );

	void IncreaseHealth( FighterID fighterID, SimVal value );
	void DecreaseHealth( FighterID fighterID, SimVal value );
	void IncreaseStamina( FighterID fighterID, SimVal value );
	void DecreaseStamina( FighterID fighterID, SimVal value );

	FighterIDs GetValidAttackTargets( FighterID attackerID ) const;
	bool CanPerformAttack( FighterID attackerID ) const;
	bool CanPerformAttack( FighterID attackerID, FighterID defenderID ) const;
	bool CanPerformAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const;
	AttackProfile PrepareAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const;
	AttackResult ExecuteAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength );

	void PassTime( FighterID initiatorID );


	//
	// Private methods
private:
	TeamEntry const& GetTeamRef( TeamID teamID ) const;
	TeamEntry& GetMutableTeamRef( TeamID teamID );
	PartyEntry const& GetPartyRef( PartyID partyID ) const;
	PartyEntry& GetMutablePartyRef( PartyID partyID );
	FighterEntry const& GetFighterRef( FighterID fighterID ) const;
	FighterEntry& GetMutableFighterRef( FighterID fighterID );

	void LoadFighterFromCharacter( Fighter& fighter, state::ObjectRef const& character );
	void SaveFighterToCharacter( state::MutableObjectRef const& character, Fighter const& fighter ) const;

	void InitializeFighter( Fighter& fighter ) const;


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
