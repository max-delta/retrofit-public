#pragma once
#include "rftl/cstdint"
#include "rftl/cstddef"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

using HashVal64 = uint64_t;

struct DirectHash
{
	HashVal64 operator() ( HashVal64 const& key ) const;
};

struct NullTerminatedStringHash
{
	HashVal64 operator() ( char const* const buffer ) const;
};

// This hash function will not change based on platform
HashVal64 StableHashBytes( rftl::nullptr_t, size_t );
HashVal64 StableHashBytes( void const* buffer, size_t length );

///////////////////////////////////////////////////////////////////////////////
}}
