#pragma once
#include "project.h"

#include "cc3o3/elements/ElementFwd.h"

#include "core_math/math_casts.h"

#include "rftl/array"

namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

using StatValue = uint8_t;
inline constexpr StatValue kMinStatValue = 0;
inline constexpr StatValue kDefaultStatValue = 0;
inline constexpr StatValue kMaxStatValue = 10;

inline constexpr size_t kMaxSlotsPerElementLevel = 8;

using ElementSlotIndex = rftl::pair<element::ElementLevel, size_t>;

enum class GridShape : uint8_t
{
	Standard = 0,
	Wide,
	Heavy,

	NumShapeTypes
};
inline constexpr uint8_t kMaxGridShapeValue = math::enum_bitcast( GridShape::NumShapeTypes ) - 1u;

struct CharData;
struct Description;
struct Genetics;
struct Visuals;
struct Stats;
struct Equipment;
struct ElementSlots;
struct GridBitMask;
struct GridSizeMask;

class CharacterDatabase;
class CharacterValidator;

///////////////////////////////////////////////////////////////////////////////
}
