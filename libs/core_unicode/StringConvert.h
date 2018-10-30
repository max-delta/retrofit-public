#pragma once
#include "rftl/string"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

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
