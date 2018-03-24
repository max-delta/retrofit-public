#include "stdafx.h"

#include "core_math/math_bytes.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathBits, ReverseByteOrder )
{
	static_assert( ReverseByteOrder( static_cast<uint8_t>( 0x11 ) ) == 0x11, "" );
	static_assert( ReverseByteOrder( static_cast<int8_t>( 0x11 ) ) == 0x11, "" );
	static_assert( ReverseByteOrder( static_cast<uint16_t>( 0x1122 ) ) == 0x2211, "" );
	static_assert( ReverseByteOrder( static_cast<int16_t>( 0x1122 ) ) == 0x2211, "" );
	static_assert( ReverseByteOrder( static_cast<uint32_t>( 0x11223344 ) ) == 0x44332211, "" );
	static_assert( ReverseByteOrder( static_cast<int32_t>( 0x11223344 ) ) == 0x44332211, "" );
	static_assert( ReverseByteOrder( static_cast<uint64_t>( 0x1122334455667788 ) ) == 0x8877665544332211, "" );
	static_assert( ReverseByteOrder( static_cast<int64_t>( 0x1122334455667788 ) ) == 0x8877665544332211, "" );
}

///////////////////////////////////////////////////////////////////////////////
}}
