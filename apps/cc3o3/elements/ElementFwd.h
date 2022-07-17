#pragma once
#include "project.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

// Element and innate identifier bytes are intended to be valid ASCII strings
// NOTE: These values are non-endian, and thus should not need byte-swapping
//  during serialization as long as they are re-interpreted as a byte array
// SEE: MakeElementIdentifier(...) and GetElementBytes(...)
// SEE: MakeInnateIdentifier(...) and GetInnateBytes(...)
using ElementIdentifier = uint64_t; // 'B' 'u' 'f' 'S' '_' 'R' '\0' '\0'
static constexpr ElementIdentifier kInvalidElementIdentifier = 0;
// NOTE: More than just 1 char for readability and to reduce collision space of
//  potential mods and user-created innates
using InnateIdentifier = uint32_t; // 'r' 'e' 'd' '\0'
static constexpr InnateIdentifier kInvalidInnateIdentifier = 0;

using ElementLevel = uint8_t;
static constexpr ElementLevel kInvalidElementLevel = 0;
static constexpr ElementLevel kMinElementLevel = 1;
static constexpr ElementLevel kMaxElementLevel = 8;
static constexpr size_t kNumElementLevels = kMaxElementLevel - kMinElementLevel + 1;

// Convenience for ranged-based for-loops
// NOTE: Defeats optimization, be mindful in performance-critical code
static constexpr ElementLevel kElementLevels[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
static_assert( sizeof( kElementLevels ) == sizeof( ElementLevel ) * kNumElementLevels );

// Convenience for zero-based container logic
static constexpr size_t AsLevelOffset( ElementLevel level )
{
	return static_cast<size_t>( level - kMinElementLevel );
}

struct ElementDesc;

///////////////////////////////////////////////////////////////////////////////
}
