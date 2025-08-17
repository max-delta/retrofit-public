#pragma once
#include "rftl/cstdlib"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Intended to be derived from when specializing rftl::formatter<...> with
//  implementations that don't accept additional args
struct parseless_format
{
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		auto const it = ctx.begin();
		if( it != ctx.end() && *it != '}' )
		{
			rftl::abort();
		}

		return it;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
