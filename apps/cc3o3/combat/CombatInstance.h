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

	// Encounter composition
	TeamID AddTeam();
	PartyID AddParty( TeamID teamID );
	FighterID AddFighter( PartyID partyID );
	void RemoveFighter( FighterID fighterID );

	// Encounter structure queries
	TeamIDs GetTeamIDs() const;
	PartyIDs GetPartyIDs() const;
	PartyIDs GetPartyIDs( TeamID teamID ) const;
	FighterIDs GetFighterIDs() const;
	FighterIDs GetFighterIDs( TeamID teamID ) const;
	FighterIDs GetFighterIDs( PartyID partyID ) const;

	// Relationship queries
	TeamIDs GetOpposingTeams( TeamID teamID ) const;

	// Combatant access
	Team GetTeam( TeamID teamID ) const;
	Party GetParty( PartyID partyID ) const;
	Fighter GetFighter( FighterID fighterID ) const;

	// Combatant setup
	void SetCombatant( FighterID fighterID, Fighter const& combatant );
	void SetCombatant( FighterID fighterID, state::MutableObjectRef const& character );

	// Get the persistent character that a combatant was sourced from
	// NOTE: Not all combatants are sourced from persistent characters, so this
	//   may return an invalidated reference
	// NOTE: The current combat data of the character may differ, as a result of
	//  changes that occurred during combat that have not been committed back to
	//  the persistent character
	state::ObjectRef GetCharacter( FighterID fighterID ) const;

	// Save and load the combat data to and from persistance
	void CommitCombatData() const;
	void ReloadCombatData();

	// Field logic
	Field::Influence GetFieldInfluence() const;
	void AddFieldInfluence( element::InnateIdentifier influence );
	void AddFieldInfluence( element::InnateIdentifier influence, size_t strength );
	void GenerateFieldInfluence( uint64_t seedHash );

	// Countering logic
	SimVal GetCounterGuage( PartyID party ) const;
	void IncreaseCounterGuage( PartyID party, SimVal value );

	// Low-level fighter modifictions, generally not meant to be used directly
	void IncreaseHealth( FighterID fighterID, SimVal value );
	void DecreaseHealth( FighterID fighterID, SimVal value );
	void IncreaseStamina( FighterID fighterID, SimVal value );
	void DecreaseStamina( FighterID fighterID, SimVal value );
	void IncreaseCharge( FighterID fighterID, SimVal value );
	void DecreaseCharge( FighterID fighterID, SimVal value );

	// Various conditions might prevent performing actions of any kind
	bool CanPerformAction( FighterID attackerID ) const;

	// Attacking logic
	FighterIDs GetValidAttackTargets( FighterID attackerID ) const;
	bool CanPerformAttack( FighterID attackerID ) const;
	bool CanPerformAttack( FighterID attackerID, FighterID defenderID ) const;
	bool CanPerformAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const;
	AttackProfile PrepareAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const;
	AttackResult ExecuteAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength );

	// Casting logic
	// NOTE: These are of limited capability, and are generally expected to be
	//  run via the casting engine, not directly on the instance
	bool CanPerformCast( FighterID attackerID ) const;
	bool CanPerformCast( FighterID attackerID, element::ElementLevel castedLevel ) const;
	bool StartCast( FighterID attackerID, element::ElementLevel castedLevel );
	bool FinishCast( FighterID attackerID );
	CastDamageProfile PrepareCastDamage(
		FighterID attackerID,
		FighterID defenderID,
		SimVal elementStrength,
		element::ElementLevel castedLevel,
		bool multiTarget,
		element::InnateIdentifier elementColor ) const;
	CastDamageResult ApplyCastDamage(
		FighterID attackerID,
		FighterID defenderID,
		SimVal elementStrength,
		element::ElementLevel castedLevel,
		bool multiTarget,
		element::InnateIdentifier elementColor );

	// Most actions cause a single unit of time to pass
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
