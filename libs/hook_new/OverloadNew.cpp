#include "stdafx.h"
#include "OverloadNew.h"

#include "core_math/math_bits.h"

#include "core/compiler.h"

#include "rftl/new"
#include "rftl/cstdlib"
#include "rftl/cstddef"


// MSVC has malformed signatures for 'new' operators
#if defined( RF_PLATFORM_MSVC ) && ( _MSC_VER >= 1916 || _MSC_VER <= 1921 )
#	define RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES __pragma( warning( suppress : 28251 ) )
#else
#	define RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES
#endif

#if RF_IS_ALLOWED( RF_CONFIG_HOOK_NEW )
///////////////////////////////////////////////////////////////////////////////
namespace RF { namespace hook { namespace details {

void* HookedObjectAllocate( size_t size, size_t align ) noexcept
{
	// The C++ standard specifies that all alignments are powers of two, and
	//  that there is a max alignment that is not overly large, so that all
	//  allocators can practically ensure alignment is always met by always
	//  aligning to the maximum value
	static constexpr bool kCheckForProperAlignments = true;
	if( kCheckForProperAlignments )
	{
		if( align > alignof( rftl::max_align_t ) )
		{
			rftl::abort();
		}

		if( math::IsPowerOfTwo( align ) == false )
		{
			rftl::abort();
		}
	}

	return GlobalObjectAllocate( size, align );
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
	void* const retVal = RF::hook::details::HookedObjectAllocate( size, 1 );
	if( retVal == nullptr )
	{
		rftl::abort();
	}
	return retVal;
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new( size_t size, rftl::nothrow_t const& nothrow ) noexcept
{
	return ::operator new( size, rftl::align_val_t( 1 ), rftl::nothrow );
}



RF_HACK_SUPPRESS_BAD_MSVC_NEW_SIGNATURES;
[[nodiscard]] void* operator new( size_t size, rftl::align_val_t align, rftl::nothrow_t const& nothrow ) noexcept
{
	return RF::hook::details::HookedObjectAllocate( size, static_cast<size_t>( align ) );
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
	RF::hook::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::align_val_t align )
{
	RF::hook::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, size_t size )
{
	RF::hook::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::nothrow_t const& nothrow )
{
	RF::hook::details::HookedObjectDeallocate( ptr );
}



void operator delete( void* ptr, rftl::align_val_t align, rftl::nothrow_t const& nothrow )
{
	RF::hook::details::HookedObjectDeallocate( ptr );
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
