#include "stdafx.h"
#include "OverloadNew.h"

#include "core/compiler.h"

#include "rftl/cstdlib"


namespace RF { namespace alloc {
#ifdef RF_PLATFORM_WINDOWS
///////////////////////////////////////////////////////////////////////////////

void* GlobalObjectAllocate( size_t size ) noexcept
{
	return malloc( size );
}



void GlobalObjectDeallocate( void* ptr ) noexcept
{
	free( ptr );
}

///////////////////////////////////////////////////////////////////////////////
#endif
}}
