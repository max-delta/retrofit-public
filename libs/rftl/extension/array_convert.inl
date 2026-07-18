#pragma once
#include "array_convert.h"

#include "rftl/tuple"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Clunky unused function parameter is just a hack to cause the template to
//  deduce the indices out into the form of a parameter pack
template<typename OutT, typename InT, size_t SizeT, size_t... IndicesT>
inline constexpr auto array_convert_by_index(
	array<InT, SizeT> const& arr,
	integer_sequence<size_t, IndicesT...> )
	-> array<OutT, SizeT>
{
	// Use the index list as passed in by template parameters to do a template
	//  paremeter pack expansion where each index causes a read and an emplace
	//  in the form of a large initializer list
	static_assert( SizeT < 8, "This is getting unreasonably large, what are you doing?" );
	return array<OutT, SizeT>{ OutT( arr[IndicesT] )... };
}

}
///////////////////////////////////////////////////////////////////////////////

template<typename OutT, typename InT, size_t SizeT>
inline constexpr auto array_convert( rftl::array<InT, SizeT> const& arr ) -> rftl::array<OutT, SizeT>
{
	// Make an empty object that is just a clunky way to coerce a bunch of
	//  templatized integers into a helper function
	using Indices = make_index_sequence<SizeT>;
	return details::array_convert_by_index<OutT>( arr, Indices{} );
}

template<typename T>
inline constexpr rftl::array<T, 0> array_convert()
{
	return {};
}

///////////////////////////////////////////////////////////////////////////////
}
