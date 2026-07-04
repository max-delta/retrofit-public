#include "stdafx.h"
#include "BlindConvert.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static_assert( sizeof( char ) == sizeof( char8_t ) );

}
///////////////////////////////////////////////////////////////////////////////

rftl::u8string_view BlindInterpretAsUtf8( rftl::string_view source )
{
	return rftl::u8string_view( BlindInterpretAsUtf8( source.data() ), source.size() );
}



char8_t const* BlindInterpretAsUtf8( char const* source )
{
	return reinterpret_cast<char8_t const*>( source );
}



char8_t* BlindInterpretAsUtf8( char* source )
{
	return reinterpret_cast<char8_t*>( source );
}

///////////////////////////////////////////////////////////////////////////////
}
