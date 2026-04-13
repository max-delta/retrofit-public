#pragma once
#include "project.h"

#include "rftl/limits"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

enum class EntityClass : uint8_t
{
	Invalid = 0,
	Peasant,
	Hero,
	Monster,
};

enum class SimColor : uint8_t
{
	Unrelated = 0,
	Same,
	Clash
};

enum class BreakClass : uint8_t
{
	Ineffective = 0,
	Weak,
	Normal,
	Critical,
	Overwhelming,
};

using SimVal = uint8_t;
using SimDelta = int8_t;
using LargeSimVal = uint16_t;
using DisplayVal = uint16_t;
using SignedDisplayVal = int16_t;

using TeamIndex = uint8_t;
inline constexpr TeamIndex kInvalidTeamIndex = rftl::numeric_limits<TeamIndex>::max();
inline constexpr size_t kMaxTeamsPerFight = 2;

using PartyIndex = uint8_t;
inline constexpr PartyIndex kInvalidPartyIndex = rftl::numeric_limits<PartyIndex>::max();
inline constexpr size_t kMaxPartiesPerTeam = 3;
inline constexpr size_t kMaxTotalParties = kMaxTeamsPerFight * kMaxPartiesPerTeam;

using FighterIndex = uint8_t;
inline constexpr FighterIndex kInvalidFighterIndex = rftl::numeric_limits<FighterIndex>::max();
inline constexpr size_t kMaxFightersPerParty = 5;
inline constexpr size_t kMaxTotalFighters = kMaxTeamsPerFight * kMaxPartiesPerTeam * kMaxFightersPerParty;

inline constexpr LargeSimVal kMinHealth = 0;
inline constexpr LargeSimVal kMaxHealth = 999;
inline constexpr SimDelta kMinStamina = -7;
inline constexpr SimDelta kMaxStamina = 7;
inline constexpr SimVal kMinCharge = 0;
inline constexpr SimVal kMaxCharge = 8;
inline constexpr SimVal kCounterGaugeFull = 100;
inline constexpr SimVal kCounterGaugeMax = 175;
inline constexpr size_t kFieldSize = 5;

struct TeamID;
struct PartyID;
struct FighterID;

struct Party;
struct Team;
struct Fighter;
struct Field;

struct AttackProfile;
struct AttackResult;

struct CastDamageProfile;
struct CastDamageResult;

class CombatEngine;
class CombatInstance;
class FightController;

///////////////////////////////////////////////////////////////////////////////
}
