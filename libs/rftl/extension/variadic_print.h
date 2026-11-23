#pragma once
#include "rftl/cstdarg"
#include "rftl/format"
#include "rftl/span"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Will ensure buffer is null-terminated somewhere, and will return the
//  characters written, INCLUDING the null-terminator, and NOT characters
//  that were truncated

// NOTE: Because C++20 format is absolutely terrible, the C++20 forms aren't
//  actually able to support anything beyond ASCII, since Unicode support isn't
//  even planned yet in the standard, which is frankly a shameful embarrassment

span<char> var_snprintf( span<char> buffer, char const* fmt, ... );

span<char> var_vsnprintf( span<char> buffer, char const* fmt, va_list args );

template<typename... ArgsT>
span<char> var_format_to_n( span<char> buffer, format_string<ArgsT...> fmt, ArgsT&&... args );

span<char> var_vformat_to( span<char> buffer, string_view fmt, format_args args );

///////////////////////////////////////////////////////////////////////////////
}

#include "variadic_print.inl"
