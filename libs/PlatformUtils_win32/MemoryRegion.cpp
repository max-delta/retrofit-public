#include "stdafx.h"
#include "project.h"

#include "core_platform/MemoryRegion.h"

#include "Logging/Logging.h"

#include "core_platform/windows_inc.h"
#include "core_math/math_clamps.h"

#include "rftl/cstdlib"


namespace RF { namespace platform {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static size_t GetPageSize()
{
	// Cache the page size, assume it can't change without rebooting
	static_assert( compiler::kAlignedModificationsAreAtomic, "Unsafe threading code" );
	static_assert( compiler::kMemoryModel == compiler::MemoryModel::Strong, "Unsafe threading code" );
	static size_t sPageSize = 0;
	if( sPageSize == 0 )
	{
		// Multiple threads may race here, but we don't care as long as the
		//  assertions above hold true
		win32::SYSTEM_INFO info = {};
		win32::GetSystemInfo( &info );
		if( info.dwPageSize == 0 )
		{
			rftl::abort();
		}
		sPageSize = info.dwPageSize;
	}
	return sPageSize;
}



static size_t GrowToPageBoundaries( size_t size )
{
	return math::SnapHighest( size, details::GetPageSize() );
}

}
///////////////////////////////////////////////////////////////////////////////

void* AcquireLargeExclusiveDataRegion( size_t size, char const* label )
{
	// At time of writing, there doesn't seem to be a good way to store this
	(void)label;

	size_t const guardSize = details::GetPageSize();
	size_t const innerRegionSize = details::GrowToPageBoundaries( size );
	size_t const totalSize = guardSize + innerRegionSize + guardSize;

	void* const head = win32::VirtualAlloc( nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE );
	void* const frontGuard = head;
	void* const retVal = reinterpret_cast<uint8_t*>( frontGuard ) + guardSize;
	void* const rearGuard = reinterpret_cast<uint8_t*>( retVal ) + innerRegionSize;

	void* const result = win32::VirtualAlloc( retVal, innerRegionSize, MEM_COMMIT, PAGE_READWRITE );
	RFLOG_TEST_AND_FATAL( result == retVal, nullptr, RFCAT_PLATFORMUTILS, "Failed to allocate data region" );

	// Note that we have reserved, but are not commiting the surrounding pages.
	//  This will cause access violations if they're touched in any way, and
	//  prevents some other unlucky memory from ending up nearby and getting
	//  stomped, but without actually paying the cost of putting real memory
	//  in that address range.
	(void)frontGuard;
	(void)rearGuard;

	return retVal;
}



void ReleaseLargeExclusiveDataRegion( void* address )
{
	size_t const guardSize = details::GetPageSize();
	void* const assumedRegionStart = reinterpret_cast<uint8_t*>( address ) - guardSize;

	win32::BOOL const result = win32::VirtualFree( assumedRegionStart, 0, MEM_RELEASE );
	RFLOG_TEST_AND_FATAL( result == win32::TRUE, nullptr, RFCAT_PLATFORMUTILS, "Failed to free data region" );
}

///////////////////////////////////////////////////////////////////////////////
}}
