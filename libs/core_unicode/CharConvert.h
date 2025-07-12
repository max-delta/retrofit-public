#pragma once

#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

bool IsValidAscii( char ch );
bool IsValidAscii( char8_t ch );
bool IsValidAscii( char16_t ch );
bool IsValidAscii( char32_t ch );

size_t NumBytesExpectedInUtf8( char8_t firstByte );
size_t NumPairsExpectedInUtf8( char16_t firstPair );

char32_t ConvertSingleUtf8ToUtf32( rftl::u8string_view source );
char32_t ConvertSingleUtf16ToUtf32( rftl::u16string_view source );

size_t ConvertSingleUtf32ToUtf8( char32_t codePoint, char8_t ( &destination )[4] );
size_t ConvertSingleUtf32ToUtf16( char32_t codePoint, char16_t ( &destination )[2] );

///////////////////////////////////////////////////////////////////////////////
}
