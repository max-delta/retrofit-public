#pragma once
#include "rftl/array"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

using IPv4Address = rftl::array<uint8_t, 4>;
static constexpr IPv4Address kIPv4Localhost = { 127, 0, 0, 1 };

using IPv6Address = rftl::array<uint8_t, 16>;
static constexpr IPv6Address kIPv6Localhost = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

///////////////////////////////////////////////////////////////////////////////
}
