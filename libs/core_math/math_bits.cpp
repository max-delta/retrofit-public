#include "stdafx.h"

#include "math_bits.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

void GetByte( void const* root, size_t offsetInBits, uint8_t const*& byte, uint8_t& mask )
{
	constexpr size_t kBitsPerByte = 8;
	size_t const offsetAsindex = offsetInBits / kBitsPerByte;

	uint8_t const* const rootByte = reinterpret_cast<uint8_t const*>( root );
	byte = &( rootByte[offsetAsindex] );

	size_t const offsetInBitsIntoByte = offsetInBits - ( offsetAsindex * kBitsPerByte );
	mask = static_cast<uint8_t>( ( 1 << 7 ) >> offsetInBitsIntoByte );
}


void GetMutableByte( void* root, size_t offsetInBits, uint8_t*& byte, uint8_t& mask )
{
	GetByte( const_cast<void const*>( root ), offsetInBits, const_cast<uint8_t const*&>( byte ), mask );
}

}
///////////////////////////////////////////////////////////////////////////////

bool IsBitSet( void const* root, size_t offsetInBits )
{
	uint8_t const* bytePtr = nullptr;
	uint8_t mask = 0;
	details::GetByte( root, offsetInBits, bytePtr, mask );
	uint8_t const& byte = *bytePtr;

	if( ( byte & mask ) == mask )
	{
		return true;
	}

	return false;
}



void SetBit( void* root, size_t offsetInBits )
{
	uint8_t* bytePtr = nullptr;
	uint8_t mask = 0;
	details::GetMutableByte( root, offsetInBits, bytePtr, mask );
	uint8_t& byte = *bytePtr;

	byte |= mask;
}



void ClearBit( void* root, size_t offsetInBits )
{
	uint8_t* bytePtr = nullptr;
	uint8_t mask = 0;
	details::GetMutableByte( root, offsetInBits, bytePtr, mask );
	uint8_t& byte = *bytePtr;

	byte &= ~mask;
}

///////////////////////////////////////////////////////////////////////////////
}
