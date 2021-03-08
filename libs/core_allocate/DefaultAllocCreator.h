#pragma once
#include "core/macros.h"
#include "core_allocate/SplitAllocCreator.h"
#include "core_allocate/EntwinedAllocCreator.h"

#include "rftl/type_traits"

namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<typename T, typename Unused = void>
class DefaultAllocCreator
{
	// HACK: Template arg prevents evaluation without substitution
	static_assert( sizeof( T ) == 0 && sizeof( T ) == 1, "Failed to determine correct creator to use" );

public:
	// NOTE: This is mostly just for Intellisense, this class isn't valid
	template<typename... U>
	static CreationPayload<T> Create( U&&... args );
};

template<typename T>
class DefaultAllocCreator<T, typename rftl::enable_if<( sizeof( T ) + sizeof( PtrRef ) < 128 ), void>::type> : public EntwinedAllocCreator<T>
{
	RF_NO_INSTANCE( DefaultAllocCreator );
	// Small types shouldn't split storage since it's wasteful on alloc count
};

template<typename T>
class DefaultAllocCreator<T, typename rftl::enable_if<( sizeof( T ) + sizeof( PtrRef ) >= 128 ), void>::type> : public SplitAllocCreator<T>
{
	RF_NO_INSTANCE( DefaultAllocCreator );
	// Large types shouldn't combine storage since it's wasteful on memory size
};

///////////////////////////////////////////////////////////////////////////////
}
