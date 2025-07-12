#pragma once

#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

size_t ConvertToASCII( char* dest, size_t destLen, rftl::string_view source );
size_t ConvertToASCII( char* dest, size_t destLen, rftl::u8string_view source );
size_t ConvertToASCII( char* dest, size_t destLen, rftl::u16string_view source );
size_t ConvertToASCII( char* dest, size_t destLen, rftl::u32string_view source );

size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::string_view source );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u8string_view source );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u16string_view source );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u32string_view source );

size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::string_view source );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u8string_view source );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u16string_view source );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u32string_view source );

size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::string_view source );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u8string_view source );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u16string_view source );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u32string_view source );

///////////////////////////////////////////////////////////////////////////////
}
