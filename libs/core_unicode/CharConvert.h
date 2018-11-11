#pragma once


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

size_t NumBytesExpectedInUtf8( char firstByte );
size_t NumPairsExpectedInUtf8( char16_t firstPair );

char32_t ConvertSingleUtf8ToUtf32( char const* buffer, size_t numBytes );
char32_t ConvertSingleUtf16ToUtf32( char16_t const* buffer, size_t numPairs );

///////////////////////////////////////////////////////////////////////////////
}}
