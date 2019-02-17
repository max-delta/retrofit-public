#pragma once
#include "VirtualCast.h"

#include "core_rftype/ClassInfoAccessor.h"

#include "core_reflect/VirtualClass.h"


namespace RF { namespace rftype {
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
	static_assert( rftl::is_base_of<reflect::VirtualClassWithoutDestructor, TargetType>::value, "This function only works with virtual lookups" );
	static_assert( rftl::is_base_of<reflect::VirtualClassWithoutDestructor, SourceT>::value, "This function only works with virtual lookups" );

	reflect::ClassInfo const& target = GetClassInfo<TargetType>();
	void const* result = source->GetVirtualClassInfo()->AttemptInheritanceWalk( target, source );

	using ConstType = typename rftl::add_const<TargetType>::type;
	ConstType* constResult = reinterpret_cast<ConstType*>( result );
	constexpr bool shouldBeConst = rftl::is_const<SourceT>::value;
	using ReturnType = typename rftl::conditional<shouldBeConst, ConstType*, TargetType*>::type;
	return const_cast<ReturnType>( constResult );
}

///////////////////////////////////////////////////////////////////////////////
}}
