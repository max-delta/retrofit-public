#pragma once
#include "project.h"

#include "rftl/limits"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

using SpawnIndex = uint8_t;
static constexpr SpawnIndex kInvalidSpawnIndex = rftl::numeric_limits<SpawnIndex>::max();
static constexpr SpawnIndex kInititialSpawnIndex = 0;
static constexpr size_t kMaxSpawns = 8;

class EncounterManager;

///////////////////////////////////////////////////////////////////////////////
}
