#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Teams, parties, and fighters all share the same number space, arranged in
//  predetermined strides
// NOTE: Base class for IDs, do not use directly
struct CombatantID
{
	//
	// Types and constants
public:
	using Value = uint8_t;
	static constexpr Value kInvalid = rftl::numeric_limits<Value>::max();

protected:
	static constexpr Value kFighterStride = 1;
	static constexpr Value kPartyStride = kFighterStride * kMaxFightersPerParty;
	static constexpr Value kTeamStride = kPartyStride * kMaxPartiesPerTeam;


	//
	// Public methods
public:
	CombatantID() = default;

	bool IsValid() const;

	// Ascend
	TeamID GetTeam() const;
	PartyID GetParty() const;

	// Self
	FighterID GetFighter() const;

	// Descend
	PartyID GetPartyAt( PartyIndex index ) const;
	FighterID GetFighterAt( FighterIndex index ) const;

	// Indices
	TeamIndex GetTeamIndex() const;
	PartyIndex GetPartyIndex() const;
	FighterIndex GetFighterIndex() const;


	//
	// Private methods
private:
	explicit CombatantID( Value value );


	//
	// Protected data
protected:
	Value mValue = kInvalid;
};

}
///////////////////////////////////////////////////////////////////////////////

struct TeamID : private details::CombatantID
{
	friend struct details::CombatantID;

	static TeamID MakeTeam( TeamIndex team );

	bool operator==( TeamID const& rhs ) const;
	bool operator!=( TeamID const& rhs ) const;

	using details::CombatantID::GetPartyAt;
	using details::CombatantID::GetTeamIndex;
};

///////////////////////////////////////////////////////////////////////////////

struct PartyID : public details::CombatantID
{
	friend struct details::CombatantID;
	static PartyID MakeParty( TeamIndex team, PartyIndex party );

	bool operator==( PartyID const& rhs ) const;
	bool operator!=( PartyID const& rhs ) const;

	using details::CombatantID::GetTeam;
	using details::CombatantID::GetFighterAt;
	using details::CombatantID::GetTeamIndex;
	using details::CombatantID::GetPartyIndex;
};

///////////////////////////////////////////////////////////////////////////////

struct FighterID : public details::CombatantID
{
	friend struct details::CombatantID;
	static FighterID MakeFighter( TeamIndex team, PartyIndex party, FighterIndex fighter );

	bool operator==( FighterID const& rhs ) const;
	bool operator!=( FighterID const& rhs ) const;

	using details::CombatantID::GetTeam;
	using details::CombatantID::GetParty;
	using details::CombatantID::GetTeamIndex;
	using details::CombatantID::GetPartyIndex;
	using details::CombatantID::GetFighterIndex;
};

///////////////////////////////////////////////////////////////////////////////
}
