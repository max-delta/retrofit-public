#pragma once

#include "rftl/array"

namespace rftl::literals {
///////////////////////////////////////////////////////////////////////////////
namespace details {
template<size_t SizeT>
struct StringToArrayConverter;
}
///////////////////////////////////////////////////////////////////////////////

// Produces a 'rftl::array<char, SizeT>' without the implicit null terminator
// NOTE: This means that if you want the null terminator, you need to specify
//  it explicitly in your string with '\0' or equivalent
template<details::StringToArrayConverter ConverterT>
consteval auto operator""_array();

///////////////////////////////////////////////////////////////////////////////
}

#include "array_string.inl"
