#pragma once
#include "project.h"

namespace RF { namespace cc { namespace combat {
///////////////////////////////////////////////////////////////////////////////

enum class EntityClass : uint8_t
{
	Peasant = 0,
	Player,
};

enum class SimColor : uint8_t
{
	Unrelated = 0,
	Same,
	Clash
};

using SimVal = uint8_t;
using SimDelta = int8_t;
using LargeSimVal = uint16_t;
using DisplayVal = uint16_t;
static constexpr size_t kFieldSize = 5;

class CombatEngine;
class CombatInstance;

///////////////////////////////////////////////////////////////////////////////
}}}
