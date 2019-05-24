#pragma once
#include "rftl/cstdint"


namespace rftl { namespace literals {
///////////////////////////////////////////////////////////////////////////////

inline constexpr uint8_t operator"" _u8( unsigned long long arg )
{
	return static_cast<uint8_t>( arg );
}



inline constexpr uint16_t operator"" _u16( unsigned long long arg )
{
	return static_cast<uint16_t>( arg );
}



inline constexpr int8_t operator"" _s8( unsigned long long arg )
{
	return static_cast<int8_t>( arg );
}



inline constexpr int16_t operator"" _s16( unsigned long long arg )
{
	return static_cast<int16_t>( arg );
}

///////////////////////////////////////////////////////////////////////////////
}}
