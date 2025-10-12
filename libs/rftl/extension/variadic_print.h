#pragma once
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
//  even planned yet in the standard, which is frankly a shameful embarrasment

string_view var_vsnprintf( char* buffer, size_t bufsz, char const* fmt, va_list args );
string_view var_vsnprintf( span<char> buffer, char const* fmt, va_list args );

template<typename... ArgsT>
string_view var_format_to_n( char* buffer, size_t bufsz, format_string<ArgsT...> fmt, ArgsT&&... args );
template<typename... ArgsT>
string_view var_format_to_n( span<char> buffer, format_string<ArgsT...> fmt, ArgsT&&... args );

string_view var_vformat_to( char* buffer, size_t bufsz, string_view fmt, format_args args );
string_view var_vformat_to( span<char> buffer, string_view fmt, format_args args );

///////////////////////////////////////////////////////////////////////////////
}

#include "variadic_print.inl"
