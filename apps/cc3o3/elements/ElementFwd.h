#pragma once
#include "project.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

// Element and innate identifier bytes are intended to be valid ASCII strings
using ElementIdentifier = uint64_t;
static constexpr ElementIdentifier kInvalidElementIdentifier = 0;
using InnateIdentifier = uint64_t;
static constexpr InnateIdentifier kInvalidInnateIdentifier = 0;

using ElementLevel = uint8_t;
static constexpr ElementLevel kInvalidElementLevel = 0;
static constexpr ElementLevel kMaxElementLevel = 8;

struct ElementDesc;

///////////////////////////////////////////////////////////////////////////////
}
