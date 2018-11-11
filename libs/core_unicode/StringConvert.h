#pragma once
#include "rftl/string"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( char const* source, size_t numBytes );
rftl::string ConvertToASCII( char16_t const* source, size_t numPairs );
rftl::string ConvertToASCII( char32_t const* source, size_t numCodePoints );

rftl::string ConvertToUtf8( char const* source, size_t numBytes );
rftl::string ConvertToUtf8( char16_t const* source, size_t numPairs );
rftl::string ConvertToUtf8( char32_t const* source, size_t numCodePoints );

rftl::u16string ConvertToUtf16( char const* source, size_t numBytes );
rftl::u16string ConvertToUtf16( char16_t const* source, size_t numPairs );
rftl::u16string ConvertToUtf16( char32_t const* source, size_t numCodePoints );

rftl::u32string ConvertToUtf32( char const* source, size_t numBytes );
rftl::u32string ConvertToUtf32( char16_t const* source, size_t numPairs );
rftl::u32string ConvertToUtf32( char32_t const* source, size_t numCodePoints );

rftl::string ConvertToASCII( rftl::string const& source );
rftl::string ConvertToASCII( rftl::u16string const& source );
rftl::string ConvertToASCII( rftl::u32string const& source );

rftl::string ConvertToUtf8( rftl::string const& source );
rftl::string ConvertToUtf8( rftl::u16string const& source );
rftl::string ConvertToUtf8( rftl::u32string const& source );

rftl::u16string ConvertToUtf16( rftl::string const& source );
rftl::u16string ConvertToUtf16( rftl::u16string const& source );
rftl::u16string ConvertToUtf16( rftl::u32string const& source );

rftl::u32string ConvertToUtf32( rftl::string const& source );
rftl::u32string ConvertToUtf32( rftl::u16string const& source );
rftl::u32string ConvertToUtf32( rftl::u32string const& source );

template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::string const& source );
template<> inline rftl::string ConvertToUtf<char>( rftl::string const& source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::string const& source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::string const& source ){return ConvertToUtf32( source );}
template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::u16string const& source );
template<> inline rftl::string ConvertToUtf<char>( rftl::u16string const& source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::u16string const& source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::u16string const& source ){return ConvertToUtf32( source );}
template<typename TargetT> rftl::basic_string<TargetT> ConvertToUtf( rftl::u32string const& source );
template<> inline rftl::string ConvertToUtf<char>( rftl::u32string const& source ){return ConvertToUtf8( source );}
template<> inline rftl::u16string ConvertToUtf<char16_t>( rftl::u32string const& source ){return ConvertToUtf16( source );}
template<> inline rftl::u32string ConvertToUtf<char32_t>( rftl::u32string const& source ){return ConvertToUtf32( source );}

///////////////////////////////////////////////////////////////////////////////
}}
