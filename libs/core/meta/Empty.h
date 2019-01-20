#pragma once
#include <rftl/type_traits>


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Sometimes you just want an empty struct for some template machinery
struct EmptyStruct
{
};
static_assert( rftl::is_empty<EmptyStruct>::value, "Unexpected type" );

namespace empty_struct_details {
// But it will still take up space if you instantiate it, as required by C++20
static_assert( sizeof( EmptyStruct ) == 1, "Unexpected size" );

// Unless you specify the [[no_unique_address]] attribute
#if 0 // NOTE: Too new for current compilers
struct ContainingClass
{
	char unused;
	[[no_unique_address]] EmptyStruct empty;
};
static_assert( sizeof( ContainingClass ) == sizeof( ContainingClass::unused ), "Unexpected size" );
#endif

struct DerivedClass : public EmptyStruct
{
	short unused;
};
// Although C++11 requires standard layouts to perform empty base optimization,
//  so you can be clever in some cases and keep the empty struct TRULY empty
static_assert( rftl::is_standard_layout<DerivedClass>::value, "Unexpected type" );
static_assert( sizeof( DerivedClass ) == sizeof( DerivedClass::unused ), "Unexpected size" );
}

///////////////////////////////////////////////////////////////////////////////
}
