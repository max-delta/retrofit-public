#include "stdafx.h"
#include "PageMapper.h"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

rftl::string PageMapper::MapTo8Bit( rftl::u32string const& codePoints ) const
{
	// HACK: Hard-coded mapping
	// TODO: Take configuration and use that instead
	rftl::string retVal;
	retVal.reserve( codePoints.size() );
	for( char32_t const& codePoint : codePoints )
	{
		if( codePoint < 127 )
		{
			retVal.push_back( static_cast<char>( codePoint ) );
		}
		else
		{
			retVal.push_back( 127 );
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
