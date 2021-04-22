#pragma once

namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Used for static asserts on template instantiations that should always fail
//  on instantiation, but should require an instantiation attempt instead of
//  blindly assuming it will always fail
template<typename T>
static constexpr bool FailTemplate()
{
	return sizeof( T ) == 0 && sizeof( T ) == 1;
}

///////////////////////////////////////////////////////////////////////////////
}
