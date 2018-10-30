#pragma once
#include "rftl/string"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

std::string ConvertToASCII( std::string const& source );
std::string ConvertToASCII( std::u16string const& source );
std::string ConvertToASCII( std::u32string const& source );

std::string ConvertToUtf8( std::string const& source );
std::string ConvertToUtf8( std::u16string const& source );
std::string ConvertToUtf8( std::u32string const& source );

std::u16string ConvertToUtf16( std::string const& source );
std::u16string ConvertToUtf16( std::u16string const& source );
std::u16string ConvertToUtf16( std::u32string const& source );

std::u32string ConvertToUtf32( std::string const& source );
std::u32string ConvertToUtf32( std::u16string const& source );
std::u32string ConvertToUtf32( std::u32string const& source );

///////////////////////////////////////////////////////////////////////////////
}}
