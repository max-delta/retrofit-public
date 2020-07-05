#pragma once
#include "project.h"

#include "rftl/limits"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

using StoryTier = uint8_t;
static constexpr StoryTier kInvalidStoryTier = 0;
static constexpr StoryTier kMaxStoryTier = 8;

using RosterIndex = uint8_t;
static constexpr RosterIndex kInvalidRosterIndex = rftl::numeric_limits<RosterIndex>::max();
static constexpr RosterIndex kInititialRosterIndex = 0;
static constexpr size_t kActiveTeamSize = 3;
static constexpr size_t kRosterSize = 8;

class CompanyManager;

///////////////////////////////////////////////////////////////////////////////
}
