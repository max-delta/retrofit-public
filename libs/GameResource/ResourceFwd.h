#pragma once

#include "rftl/cstdint"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

using ResourceTypeIdentifier = uint64_t;
static constexpr ResourceTypeIdentifier kInvalidResourceTypeIdentifier = 0;

class ResourceTypeRecord;
class ResourceLoader;

///////////////////////////////////////////////////////////////////////////////
}
