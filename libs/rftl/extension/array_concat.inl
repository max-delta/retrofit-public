#pragma once
#include "array_concat.h"

#include "rftl/tuple"
#include "rftl/type_traits"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename T, size_t... SizeT>
inline constexpr auto array_concat( rftl::array<T, SizeT> const&... arrays )
{
	// Figure out the total size
	// NOTE: Of the form '(pack op ...)'
	// SEE: https://en.cppreference.com/cpp/language/fold
	constexpr size_t kTotalSize = ( SizeT + ... );

	// Implicitly coerce all the arrays into being tuples, while simultaneously
	//  concatenating them all
	// NOTE: Look at RF::TypeList and RF::ValueList for examples of the
	//  underlying machinery that this is doing under the hood
	// SEE: https://en.cppreference.com/cpp/utility/tuple/tuple_cat
	rftl::tuple const combinedTuple = rftl::tuple_cat( arrays... );

	// Use a tuple-unpacking helper that coerces the tuple back into a
	//  parameter pack
	// NOTE: Look at RF::TypeList and RF::ValueList for examples of the
	//  underlying machinery that this is doing under the hood
	// SEE: https://en.cppreference.com/cpp/utility/apply
	return rftl::apply(
		[]<typename... InnerT>( InnerT const&... elems ) -> rftl::array<T, kTotalSize>
		{
			// Assert that every parameter is all the same type, just for the
			//  sake of clarity
			// NOTE: Of the form '(pack op ...)'
			// SEE: https://en.cppreference.com/cpp/language/fold
			static_assert( ( rftl::is_same<T, InnerT>::value && ... ) );

			// Expand the parameter pack out as an initializer list to the
			//  array constructor
			// SEE: https://en.cppreference.com/cpp/language/pack
			return rftl::array<T, kTotalSize>{ elems... };
		},
		combinedTuple );
}

template<typename T>
constexpr rftl::array<T, 0> array_concat()
{
	return {};
}

///////////////////////////////////////////////////////////////////////////////
}
