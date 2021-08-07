#pragma once
#include <cstdint>


namespace RF::platform::thread {
///////////////////////////////////////////////////////////////////////////////

enum class ThreadPriority : uint8_t
{
	Invalid = 0,
	High,
	Normal,
	Low
};

// NOTE: Implementation-defined, requires additional linkage to a platform
void SetThreadName( char const* label );
void SetThreadPriority( ThreadPriority priority );

///////////////////////////////////////////////////////////////////////////////
}
