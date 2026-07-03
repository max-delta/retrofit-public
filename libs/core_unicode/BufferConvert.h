#pragma once

#include "rftl/span"
#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// Buffer converts read each character from the source, and write to the
//  destination until either they run out of room or there is nothing left to
//  read from the source
// NOTE: Returns the number of characters that were read and would have been
//  written, which may be more than what was actually written, or less than
// 	the buffer could have held
// NOTE: Nulls are handled like any other character, and are given no special
//  meaning during process, so will not act like terminators
// NOTE: No additional null terminator is added at the end

size_t ConvertToASCII( rftl::span<char> dest, rftl::string_view source );
size_t ConvertToASCII( rftl::span<char> dest, rftl::u8string_view source );
size_t ConvertToASCII( rftl::span<char> dest, rftl::u16string_view source );
size_t ConvertToASCII( rftl::span<char> dest, rftl::u32string_view source );

size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::string_view source );
size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u8string_view source );
size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u16string_view source );
size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u32string_view source );

size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::string_view source );
size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u8string_view source );
size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u16string_view source );
size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u32string_view source );

size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::string_view source );
size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u8string_view source );
size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u16string_view source );
size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u32string_view source );

///////////////////////////////////////////////////////////////////////////////
}
