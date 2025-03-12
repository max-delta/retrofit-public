#pragma once
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Will ensure buffer is null-terminated somewhere, and will return the
//  characters written, INCLUDING the null-terminator, and NOT characters
//  that were truncated
string_view var_snprintf( char* buffer, size_t bufsz, char const* fmt, va_list args );

///////////////////////////////////////////////////////////////////////////////
}

#include "variadic_print.inl"
