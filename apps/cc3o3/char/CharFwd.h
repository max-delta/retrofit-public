#pragma once
#include "project.h"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

static constexpr size_t kMaxElementLevels = 8;
static constexpr size_t kMaxSlotsPerElementLevel = 8;

struct Character;
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
