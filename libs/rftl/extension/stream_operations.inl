#pragma once
#include "stream_operations.h"

#include "rftl/ios"
#include "rftl/limits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Stream>
bool stream_seek_abs( Stream& source, size_t position )
{
	if( position > static_cast<size_t>( rftl::numeric_limits<rftl::streamoff>::max() ) )
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
	if( count > static_cast<size_t>( rftl::numeric_limits<rftl::streamsize>::max() ) )
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
