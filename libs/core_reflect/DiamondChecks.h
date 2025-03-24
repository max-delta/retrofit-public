#pragma once
#include "core/macros.h"

#include "rftl/type_traits"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

// Convert from a derived pointer to a base, using compiler-generated code to
//  handle any complexities that may arrive, such as multiple inheritance cases
// NOTE: Primarily intended to be used as generated helper functions that can
//  store pointers to the functions to be called later, in cases where the
//  calling code only has access to untyped pointers
template<typename BASE, typename DERIVED>
inline void const* GetBasePointerFromDerivedUntyped( void const* ptr )
{
	static_assert( rftl::is_base_of<BASE, DERIVED>::value, "Unrelated pointer types" );
	// NOTE: rftl::is_pointer_interconvertible_base_of may or may not be true,
	//  so this might result in a simple no-op, or it might generate code to
	//  meander about in virtual tables
	return static_cast<BASE const*>( reinterpret_cast<DERIVED const*>( ptr ) );
}



// The compiler knows whether it will need to generate code to do pointer
//  tranformation to handle poorly-written code with multiple inheritance, or
//  if it's normal types that don't require any change in the pointer address
template<typename BASE, typename DERIVED>
inline constexpr bool IsBasePointerAlwaysSameAsDerivedPointer()
{
	RF_CPP20_TODO( "Implement with C++20 trait is_pointer_interconvertible_base_of" );
#if defined( RF_PLATFORM_CLANG )
	static_assert( __has_cpp_attribute( __cpp_lib_is_pointer_interconvertible ) == false, "Check if Clang supports C++20 now" );
#else
	static_assert( __cpp_lib_is_pointer_interconvertible == 201907 );
#endif
	//return rftl::is_pointer_interconvertible_base_of<BASE, DERIVED>::value;
	return false;
}



// In any decent code, conversions between inherited class shouldn't require
//  any kind of pointer transformations, but in poor code that uses multiple
//  inheritance non-trivial transformations may be needed
// NOTE: Performing this transformation test requires the derived class to be
//  cast to the base class. This is normally required for type-safe casts, and
//  reflection COULD work around it with enough information, but that creates
//  dangerous assumptions, which this test is trying to detect.
// NOTE: In some inheritance implementations, the act of casting an invalid
//  pointer creates run-time code that will crash, so you will need to have a
//  mechanism to avoid invoking this in those situations
template<typename BASE, typename DERIVED>
inline bool UnsafeTryDetermineIfNonTrivialPointerTransformNeeded()
{
	static_assert( rftl::is_base_of<BASE, DERIVED>::value, "Unrelated pointer types" );
	// Dereferencing invalid memory isn't strictly impossible (though a clear
	//  standards violation), as it could just do some pointer math and never
	//  attempt to touch the memory, but in complex multiple inheritance cases,
	//  it can require a lookup into a virtual table, which would cause a crash
	//  when it tries to touch the memory
	DERIVED const* const invalidClass = reinterpret_cast<DERIVED const*>( compiler::kInvalidNonNullPointer );
	// If you crash on this line, check the caller, it should provide a
	//  mechanism for you to avoid this codepath for your specific types
	BASE const* const dangerousCast = static_cast<BASE const*>( invalidClass );
	return reinterpret_cast<void const*>( dangerousCast ) != reinterpret_cast<void const*>( invalidClass );
}

///////////////////////////////////////////////////////////////////////////////
}
