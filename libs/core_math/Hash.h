#pragma once
#include <stdint.h>

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

using HashVal64 = uint64_t;

struct DirectHash
{
	HashVal64 operator() (HashVal64 const& key) const
	{
		return key;
	}
};

// This hash function will not change based on platform
HashVal64 StableHashBytes( void const* buffer, size_t length );

///////////////////////////////////////////////////////////////////////////////
}}
