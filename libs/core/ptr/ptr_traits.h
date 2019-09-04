#pragma once
#include "rftl/type_traits"


#define RF_PTR_ASSERT_DELETABLE( TYPE ) \
	static_assert( \
		rftl::is_polymorphic<TYPE>::value == false || \
			rftl::has_virtual_destructor<TYPE>::value || \
			rftl::is_base_of<::RF::PtrTrait::NoVirtualDestructor, TYPE>::value, \
		"Potentially unsafe deletions due to non-virtual destructor on base class. Please use 'virtual ~Base() = default;' at a minimum." );

#define RF_PTR_ASSERT_CASTABLE( BASETYPE, DERIVEDTYPE ) \
	static_assert( \
		rftl::is_base_of<BASETYPE, DERIVEDTYPE>::value || \
		( rftl::is_const<BASETYPE>::value && rftl::is_same<BASETYPE, void const>::value ) || \
		( rftl::is_same<rftl::add_const<BASETYPE>::type, void const>::value ), \
		"Failed to cast PTR<" #DERIVEDTYPE "> to PTR<" #BASETYPE ">" ); \
	RF_PTR_ASSERT_DELETABLE( BASETYPE );

namespace RF { namespace PtrTrait {
///////////////////////////////////////////////////////////////////////////////

struct NoVirtualDestructor
{
};

///////////////////////////////////////////////////////////////////////////////
}}
