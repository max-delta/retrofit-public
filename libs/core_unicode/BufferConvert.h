#pragma once


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

size_t ConvertToASCII( char* dest, size_t destLen, char const* source, size_t numBytes );
size_t ConvertToASCII( char* dest, size_t destLen, char8_t const* source, size_t numPairs );
size_t ConvertToASCII( char* dest, size_t destLen, char16_t const* source, size_t numPairs );
size_t ConvertToASCII( char* dest, size_t destLen, char32_t const* source, size_t numCodePoints );

size_t ConvertToUtf8( char8_t* dest, size_t destLen, char const* source, size_t numBytes );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, char8_t const* source, size_t numPairs );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, char16_t const* source, size_t numPairs );
size_t ConvertToUtf8( char8_t* dest, size_t destLen, char32_t const* source, size_t numCodePoints );

size_t ConvertToUtf16( char16_t* dest, size_t destLen, char const* source, size_t numBytes );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, char8_t const* source, size_t numPairs );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, char16_t const* source, size_t numPairs );
size_t ConvertToUtf16( char16_t* dest, size_t destLen, char32_t const* source, size_t numCodePoints );

size_t ConvertToUtf32( char32_t* dest, size_t destLen, char const* source, size_t numBytes );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, char8_t const* source, size_t numPairs );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, char16_t const* source, size_t numPairs );
size_t ConvertToUtf32( char32_t* dest, size_t destLen, char32_t const* source, size_t numCodePoints );

///////////////////////////////////////////////////////////////////////////////
}
