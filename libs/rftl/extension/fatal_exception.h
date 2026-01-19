#pragma once
#include "rftl/cstdlib"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// At time of writing, LLVM doesn't support throws, which is absolutely
//  infuriating, so we're just aborting here
template<typename ExceptionT>
void fatal_exception( ExceptionT const& exception )
{
	( (void)exception );
	rftl::abort();
}

///////////////////////////////////////////////////////////////////////////////
}
