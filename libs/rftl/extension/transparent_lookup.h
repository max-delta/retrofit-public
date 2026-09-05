#pragma once
#include "rftl/extension/fatal_exception.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename ContainerT, typename LookupT>
auto transparent_at( ContainerT& container, LookupT& lookup )
{
	static_assert( __cplusplus < 202602L, "Review this" );
	//return container.at( lookup );
	auto const iter = container.find( lookup );
	if( iter == container.end() )
	{
		fatal_exception( "out of range" );
	}
	return iter->second;
}



template<typename ContainerT, typename LookupT>
auto transparent_erase( ContainerT& container, LookupT& lookup )
{
	static_assert( __cplusplus < 202602L, "Review this" );
	//return container.erase( lookup );
	auto const iter = container.find( lookup );
	return container.erase( iter );
}

///////////////////////////////////////////////////////////////////////////////
}
