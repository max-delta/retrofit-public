#pragma once

#include "rftl/array"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Converts an array from one type to another type by emplacing elements
template<typename OutT, typename InT, size_t SizeT>
constexpr auto array_convert( rftl::array<InT, SizeT> const& arr ) -> rftl::array<OutT, SizeT>;

// Degenerate case
template<typename T>
constexpr rftl::array<T, 0> array_convert();

///////////////////////////////////////////////////////////////////////////////
}

#include "array_convert.inl"
