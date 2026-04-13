#pragma once
#include "project.h"

#include "rftl/limits"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

using SpawnIndex = uint8_t;
inline constexpr SpawnIndex kInvalidSpawnIndex = rftl::numeric_limits<SpawnIndex>::max();
inline constexpr SpawnIndex kInititialSpawnIndex = 0;
inline constexpr size_t kMaxSpawns = 8;

class EncounterManager;

///////////////////////////////////////////////////////////////////////////////
}
