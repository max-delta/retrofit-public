#include "stdafx.h"

#include "vector.h"

#include "rftl/deque"


namespace RF { namespace rftype { namespace extensions {
///////////////////////////////////////////////////////////////////////////////
namespace vector_accessor_details {
static rftl::deque<size_t> gStableKeys;
size_t const* GetStableKey( size_t key )
{
	// TODO: This whole thing is foul, come up with better mechanism
	if( key + 1 > gStableKeys.size() )
	{
		gStableKeys.resize( key + 1 );
		RF_ASSERT_MSG( gStableKeys.size() < 1024, "Seriously reconsider this nonsense lookup table!" );
	}
	gStableKeys[key] = key;
	return &gStableKeys.at( key );
}
}
///////////////////////////////////////////////////////////////////////////////
}}}
