#pragma once
#include "project.h"

#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"

namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

static constexpr size_t kMaxElementLevels = element::kMaxElementLevel;
static constexpr size_t kMaxSlotsPerElementLevel = 8;

using SlotsPerElemLevel = rftl::array<size_t, kMaxElementLevels>;

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
}}}
