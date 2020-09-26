#include "stdafx.h"
#include "project.h"

#include "core_platform/MemoryRegion.h"

#include "Logging/Logging.h"

#include "core_platform/inc/windows_inc.h"
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



void const* AcquireLargeDynamicHeapHandle( size_t maxSize, char const* label, bool threadSafe )
{
	win32::DWORD flags = 0;
	if( threadSafe == false )
	{
		flags |= HEAP_NO_SERIALIZE;
	}
	win32::HANDLE const handle = win32::HeapCreate( flags, 0, maxSize );
	RFLOG_TEST_AND_FATAL( handle != nullptr, nullptr, RFCAT_PLATFORMUTILS, "Failed to create heap" );
	return handle;
}



void ReleaseLargeDynamicHeapHandle( void const* heapHandle )
{
	win32::BOOL const result = win32::HeapDestroy( const_cast<void*>( heapHandle ) );
	RFLOG_TEST_AND_FATAL( result == win32::TRUE, nullptr, RFCAT_PLATFORMUTILS, "Failed to destroy heap" );
}



void* AllocateInLargeDynamicHeap( void const* heapHandle, size_t size )
{
	void* const allocation = win32::HeapAlloc( const_cast<void*>( heapHandle ), 0, size );
	RFLOG_TEST_AND_FATAL( allocation != nullptr, nullptr, RFCAT_PLATFORMUTILS, "Failed to allocate from heap" );
	return allocation;
}



void DeleteInLargeDynamicHeap( void const* heapHandle, void* address )
{
	win32::BOOL const result = win32::HeapFree( const_cast<void*>( heapHandle ), 0, address );
	RFLOG_TEST_AND_FATAL( result == win32::TRUE, nullptr, RFCAT_PLATFORMUTILS, "Failed to free from heap" );
}



size_t QueryLargeDynamicHeapSize( void const* heapHandle )
{
	// Flags aren't documented...
	static constexpr win32::DWORD kUndocumentedFlags = 0;

	win32::HEAP_SUMMARY summary = {};

	// There's an undocumented requirement for the function to succeed, which
	//  can be figured out by reverse-engineering the instructions inside...
	static constexpr win32::DWORD kUndocumentedValueRequirement =
		compiler::kArchitecture == compiler::Architecture::x86_64 ? 0x28u :
		compiler::kArchitecture == compiler::Architecture::x86_32 ? 0x14u :
		0x0;
	win32::DWORD* const undocumentedConditionalTarget = reinterpret_cast<win32::DWORD*>( &summary );
	*undocumentedConditionalTarget = kUndocumentedValueRequirement;

	// NOTE: Documentation says this is supposed to be treated like an HRESULT,
	//  even though it's a BOOL. And although other buggy parts of the API have
	//  this problem, this one is actually supposed to be a BOOL
	win32::BOOL const result = win32::HeapSummary( const_cast<void*>( heapHandle ), kUndocumentedFlags, &summary );
	RFLOG_TEST_AND_FATAL( result == win32::TRUE, nullptr, RFCAT_PLATFORMUTILS, "Failed to query heap" );
	return summary.cbAllocated;
}

///////////////////////////////////////////////////////////////////////////////
}}
