#pragma once
#include "project.h"

#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"

namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

static constexpr size_t kMaxSlotsPerElementLevel = 8;

using SlotsPerElemLevel = rftl::array<size_t, element::kNumElementLevels>;
using ElementSlotIndex = rftl::pair<element::ElementLevel, size_t>;

struct CharData;
struct Description;
struct Genetics;
struct Visuals;
struct Stats;
struct Equipment;
struct ElementSlots;

class CharacterDatabase;
class CharacterValidator;

///////////////////////////////////////////////////////////////////////////////
}
