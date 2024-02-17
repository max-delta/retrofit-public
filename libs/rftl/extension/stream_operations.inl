#pragma once
#include "stream_operations.h"

#include "rftl/ios"
#include "rftl/limits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////
namespace details {
// This obnoxious shim is to workaround the issue where clang complains about
//  comparisons that will always return the same result, and complains even if
//  you guard that path with constexpr branches to prevent it
// NOTE: The core math library has better tools to deal with this, and the core
//  library has pragma helpers for suppressing warnings, but neither of those
//  are currently supposed to be depended on by RFTL
template<typename LHS, typename RHS>
inline bool less_than_32bit_clang_workaround( LHS const& lhs, RHS const& rhs )
{
	return lhs < rhs;
}
}
///////////////////////////////////////////////////////////////////////////////

template<typename Stream>
bool stream_seek_abs( Stream& source, size_t position )
{
	if(
		details::less_than_32bit_clang_workaround(
			static_cast<size_t>( rftl::numeric_limits<rftl::streamoff>::max() ),
			position ) )
	{
		return false;
	}
	rftl::streamoff const bustedPos = static_cast<rftl::streamoff>( position );
	source.seekg( bustedPos, rftl::ios_base::beg );

	if( source.fail() )
	{
		return false;
	}

	return true;
}



template<typename Stream>
inline size_t stream_read( void* dest, size_t count, Stream& source )
{
	if(
		details::less_than_32bit_clang_workaround(
			static_cast<size_t>( rftl::numeric_limits<rftl::streamsize>::max() ),
			count ) )
	{
		return 0;
	}
	rftl::streamsize const bustedCount = static_cast<rftl::streamsize>( count );

	source.read( reinterpret_cast<char*>( dest ), bustedCount );
	rftl::streamsize const bustedResult = source.gcount();
	if( bustedResult < 0 )
	{
		return 0;
	}
	size_t const result = static_cast<size_t>( bustedResult );
	if( result > count )
	{
		return 0;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
}
