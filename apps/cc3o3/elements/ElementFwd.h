#pragma once
#include "project.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

// Element and innate identifier bytes are intended to be valid ASCII strings
using ElementIdentifier = uint64_t; // 'B' 'u' 'f' 'S' '_' 'R' '\0' '\0'
static constexpr ElementIdentifier kInvalidElementIdentifier = 0;
using InnateIdentifier = uint32_t; // 'r' 'e' 'd' '\0'
static constexpr InnateIdentifier kInvalidInnateIdentifier = 0;

using ElementLevel = uint8_t;
static constexpr ElementLevel kInvalidElementLevel = 0;
static constexpr ElementLevel kMaxElementLevel = 8;

struct ElementDesc;

///////////////////////////////////////////////////////////////////////////////
}
