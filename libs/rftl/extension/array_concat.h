#pragma once

#include "rftl/array"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Concatenates multiple arrays by copying elements
template<typename T, size_t... SizeT>
constexpr auto array_concat( rftl::array<T, SizeT> const&... arrays );

// Degenerate case
template<typename T>
constexpr rftl::array<T, 0> array_concat();

///////////////////////////////////////////////////////////////////////////////
}

#include "array_concat.inl"
