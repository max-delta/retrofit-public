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
static constexpr ElementLevel kMaxElementLevel = 8;

struct ElementDesc;

///////////////////////////////////////////////////////////////////////////////
}
