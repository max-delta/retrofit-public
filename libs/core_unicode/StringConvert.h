#pragma once
#include "rftl/string"
#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( char const* source, size_t numBytes );
rftl::string ConvertToASCII( char8_t const* source, size_t numPairs );
rftl::string ConvertToASCII( char16_t const* source, size_t numPairs );
rftl::string ConvertToASCII( char32_t const* source, size_t numCodePoints );

rftl::u8string ConvertToUtf8( char const* source, size_t numBytes );
rftl::u8string ConvertToUtf8( char8_t const* source, size_t numPairs );
rftl::u8string ConvertToUtf8( char16_t const* source, size_t numPairs );
rftl::u8string ConvertToUtf8( char32_t const* source, size_t numCodePoints );

rftl::u16string ConvertToUtf16( char const* source, size_t numBytes );
rftl::u16string ConvertToUtf16( char8_t const* source, size_t numPairs );
rftl::u16string ConvertToUtf16( char16_t const* source, size_t numPairs );
rftl::u16string ConvertToUtf16( char32_t const* source, size_t numCodePoints );

rftl::u32string ConvertToUtf32( char const* source, size_t numBytes );
rftl::u32string ConvertToUtf32( char8_t const* source, size_t numPairs );
rftl::u32string ConvertToUtf32( char16_t const* source, size_t numPairs );
rftl::u32string ConvertToUtf32( char32_t const* source, size_t numCodePoints );

rftl::string ConvertToASCII( rftl::string_view source );
rftl::string ConvertToASCII( rftl::u8string_view source );
rftl::string ConvertToASCII( rftl::u16string_view source );
rftl::string ConvertToASCII( rftl::u32string_view source );

rftl::u8string ConvertToUtf8( rftl::string_view source );
rftl::u8string ConvertToUtf8( rftl::u8string_view source );
rftl::u8string ConvertToUtf8( rftl::u16string_view source );
rftl::u8string ConvertToUtf8( rftl::u32string_view source );

rftl::u16string ConvertToUtf16( rftl::string_view source );
rftl::u16string ConvertToUtf16( rftl::u8string_view source );
rftl::u16string ConvertToUtf16( rftl::u16string_view source );
rftl::u16string ConvertToUtf16( rftl::u32string_view source );

rftl::u32string ConvertToUtf32( rftl::string_view source );
rftl::u32string ConvertToUtf32( rftl::u8string_view source );
rftl::u32string ConvertToUtf32( rftl::u16string_view source );
rftl::u32string ConvertToUtf32( rftl::u32string_view source );

template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::u8string_view source );
template<> inline rftl::u8string ConvertToUtf<char8_t>( rftl::u8string_view source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::u8string_view source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::u8string_view source ){return ConvertToUtf32( source );}
template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::u16string_view source );
template<> inline rftl::u8string ConvertToUtf<char8_t>( rftl::u16string_view source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::u16string_view source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::u16string_view source ){return ConvertToUtf32( source );}
template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::u32string_view source );
template<> inline rftl::u8string ConvertToUtf<char8_t>( rftl::u32string_view source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::u32string_view source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::u32string_view source ){return ConvertToUtf32( source );}

///////////////////////////////////////////////////////////////////////////////
}
