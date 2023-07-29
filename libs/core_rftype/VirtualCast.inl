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
	static_assert( rftl::is_base_of<SourceT, TargetType>::value, "This function is for going from base class pointers to derived class instances" );

	if( source == nullptr )
	{
		// Trivial case
		// NOTE: Avoids cost of walking inheritance trees
		return nullptr;
	}

	// Figure out our target
	// NOTE: Should be the same or a child of our source pointer type, as
	//  ensured by the is_base_of<SourceT, TargetType> assert above
	// NOTE: Should be the same or a parent of our true instance, otherwise it
	//  isn't a valid conversion and we're going to return null at the end
	reflect::ClassInfo const& target = GetClassInfo<UnconstTargetType>();

	// Figure out our true instance
	reflect::ClassInfo const& mostDerivedClassInfo = *source->GetVirtualClassInfo();

	// Try to get to our target from our source
	// NOTE: Inheritance chain is SourceT <= TargetT <= *source (vtable)
	// NOTE: May return null, the same location, or a different location (such as
	//  from implementation-specific handling of multiple inheritance)
	// NOTE: If the cast was perfectly predicted, and the instance is the same
	//  type as the target type, then the cast will trivially return the
	//  pointer back, otherwise it'll upcast repeatedly through the inheritance
	//  graph (could be multiple inheritance) trying to find the target type
	void const* result = mostDerivedClassInfo.AttemptUpcastWalk( target, source );

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
