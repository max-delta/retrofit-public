#pragma once
#include "project.h"

#include "rftl/limits"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

using StoryTier = uint8_t;
inline constexpr StoryTier kInvalidStoryTier = 0;
inline constexpr StoryTier kMaxStoryTier = 8;

using RosterIndex = uint8_t;
inline constexpr RosterIndex kInvalidRosterIndex = rftl::numeric_limits<RosterIndex>::max();
inline constexpr RosterIndex kInititialRosterIndex = 0;
inline constexpr size_t kActiveTeamSize = 3;
inline constexpr size_t kRosterSize = 8;

class CompanyManager;

///////////////////////////////////////////////////////////////////////////////
}
