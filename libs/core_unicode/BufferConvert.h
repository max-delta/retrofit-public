#pragma once

#include "rftl/span"
#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

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
