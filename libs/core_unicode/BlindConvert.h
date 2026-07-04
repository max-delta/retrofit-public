#pragma once
#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

rftl::u8string_view BlindInterpretAsUtf8( rftl::string_view source );
char8_t const* BlindInterpretAsUtf8( char const* source );
char8_t* BlindInterpretAsUtf8( char* source );

///////////////////////////////////////////////////////////////////////////////
}
