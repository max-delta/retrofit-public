#pragma once
#include "VirtualCast.h"

#include "core_rftype/ClassInfoAccessor.h"

#include "core_reflect/VirtualClass.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool IsSameOrDerivedFrom( reflect::ClassInfo const& descendent )
{
	static_assert( rftl::is_base_of<reflect::VirtualClassWithoutDestructor, T>::value, "This function only works with virtual lookups" );
	reflect::ClassInfo const& ancestor = GetClassInfo( T );
	return descendent.IsSameOrDerivedFrom( ancestor );
}


template<typename TargetT, typename SourceT>
TargetT virtual_cast( SourceT* source )
{
	static_assert( rftl::is_pointer<TargetT>::value, "Expected a pointer as template argument" );
	using TargetType = typename rftl::remove_pointer<TargetT>::type;
	using UnconstTargetType = typename rftl::remove_cv<TargetType>::type;
	static_assert( rftl::is_base_of<reflect::VirtualClassWithoutDestructor, TargetType>::value, "This function only works with virtual lookups" );
	static_assert( rftl::is_base_of<reflect::VirtualClassWithoutDestructor, SourceT>::value, "This function only works with virtual lookups" );

	if( source == nullptr )
	{
		// Trivial case
		// NOTE: Avoids cost of walking inheritance trees
		return nullptr;
	}

	// Figure out our target
	reflect::ClassInfo const& target = GetClassInfo<UnconstTargetType>();

	// Try to get to our target from our source
	// NOTE: May return null, the same location, or a different location (such as
	//  from implementation-specific handling of multiple inheritance)
	void const* result = source->GetVirtualClassInfo()->AttemptInheritanceWalk( target, source );

	// Ensure we enforce return const-ness based on the source type's const-ness
	using ConstType = typename rftl::add_const<TargetType>::type;
	ConstType* constResult = reinterpret_cast<ConstType*>( result );
	constexpr bool shouldBeConst = rftl::is_const<SourceT>::value;
	using ReturnType = typename rftl::conditional<shouldBeConst, ConstType*, TargetType*>::type;
	ReturnType const retVal = const_cast<ReturnType>( constResult );

	// NOTE: This will fail to compile if it would result in const being dropped
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
