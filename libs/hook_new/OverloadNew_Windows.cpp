#include "stdafx.h"
#include "OverloadNew.h"

#include "core/compiler.h"

#include "rftl/cstdlib"


namespace RF::hook {
#ifdef RF_PLATFORM_WINDOWS
///////////////////////////////////////////////////////////////////////////////

void* GlobalObjectAllocate( size_t size, size_t align ) noexcept
{
	// Malloc is defined to be aligned for all valid alignments
	(void)align;

	return malloc( size );
}



void GlobalObjectDeallocate( void* ptr ) noexcept
{
	free( ptr );
}

///////////////////////////////////////////////////////////////////////////////
#endif
}
