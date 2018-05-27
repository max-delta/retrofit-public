#pragma once
#include "core/macros.h"
#include "core/ptr/split_creator.h"
#include "core/ptr/entwined_creator.h"

#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T, typename Unused = void>
class DefaultCreator
{
	// HACK: Template arg prevents evaluation without substitution
	static_assert( sizeof(T) == 0 && sizeof(T) == 1, "Failed to find determine correct creator to use");
};

template<typename T>
class DefaultCreator< T, typename rftl::enable_if<(sizeof(T) + sizeof( PtrRef ) < 128), void>::type > : public EntwinedCreator<T>
{
	// Small types shouldn't split storage since it's wasteful on alloc count
};

template<typename T>
class DefaultCreator< T, typename rftl::enable_if<(sizeof( T ) + sizeof( PtrRef ) >= 128), void>::type > : public SplitCreator<T>
{
	// Large types shouldn't combine storage since it's wasteful on memory size
};

///////////////////////////////////////////////////////////////////////////////
}
