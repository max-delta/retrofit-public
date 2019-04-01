#include "stdafx.h"
#include "OverloadNew.h"

#include "core/compiler.h"

#include "rftl/new"
#include "rftl/cstdlib"


// MSVC has malformed signatures for 'new' operators
#if defined( RF_PLATFORM_MSVC ) && _MSC_VER == 1916
#	define RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES __pragma( warning( suppress : 28251 ) )
#else
#	define RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES
#endif

#if RF_IS_ALLOWED( RF_CONFIG_HOOK_NEW )
///////////////////////////////////////////////////////////////////////////////
namespace RF { namespace alloc { namespace details {

void* HookedObjectAllocate( size_t size ) noexcept
{
	return GlobalObjectAllocate( size );
}



void HookedObjectDeallocate( void* ptr ) noexcept
{
	GlobalObjectDeallocate( ptr );
}

}}}
///////////////////////////////////////////////////////////////////////////////

RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new( size_t size )
{
	void* const retVal = RF::alloc::details::HookedObjectAllocate( size );
	if( retVal == nullptr )
	{
		rftl::abort();
	}
	return retVal;
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new( size_t size, rftl::nothrow_t const& nothrow ) noexcept
{
	return RF::alloc::details::HookedObjectAllocate( size );
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new( size_t size, rftl::align_val_t align, rftl::nothrow_t const& nothrow ) noexcept
{
	// TODO: Implement
	return nullptr;
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new[]( size_t size )
{
	return ::operator new( size );
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new[]( size_t size, rftl::nothrow_t const& nothrow ) noexcept
{
	return ::operator new( size, nothrow );
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new[]( size_t size, rftl::align_val_t align, rftl::nothrow_t const& nothrow ) noexcept
{
	return ::operator new( size, align, nothrow );
}



void operator delete( void* ptr )
{
	RF::alloc::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::align_val_t align )
{
	RF::alloc::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, size_t size )
{
	RF::alloc::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::nothrow_t const& nothrow )
{
	RF::alloc::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::align_val_t align, rftl::nothrow_t const& nothrow )
{
	RF::alloc::details::HookedObjectDeallocate( ptr );
}



void operator delete[]( void* ptr )
{
	::operator delete( ptr );
}



void operator delete[]( void* ptr, rftl::align_val_t align )
{
	::operator delete( ptr, align );
}



void operator delete[]( void* ptr, size_t size )
{
	::operator delete( ptr, size );
}



void operator delete[]( void* ptr, rftl::nothrow_t const& nothrow )
{
	::operator delete( ptr, nothrow );
}



void operator delete[]( void* ptr, rftl::align_val_t align, rftl::nothrow_t const& nothrow )
{
	::operator delete( ptr, align, nothrow );
}

///////////////////////////////////////////////////////////////////////////////
#endif
