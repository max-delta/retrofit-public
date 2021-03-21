#pragma once
#include "BitField.h"

#include "core/compiler.h"
#include "core_math/math_bits.h"
#include "core_math/math_bytes.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<size_t... fieldSizes>
inline void const* BitField<fieldSizes...>::Data() const
{
	return &mStorage[0];
}



template<size_t... fieldSizes>
template<size_t index, typename AccessTypeT>
inline AccessTypeT BitField<fieldSizes...>::ReadAt() const
{
	using AccessType = AccessTypeT;

	constexpr size_t kBytesBeforeIndex = GetBytesBeforeIndex<index>();
	constexpr size_t kBitsOffsetIntoIndex = GetBitsOffsetIntoIndex<index>();
	constexpr size_t kBitsAtIndex = GetBitsAtIndex<index>();
	static_assert( sizeof( AccessType ) * 8 >= kBitsAtIndex, "Field has more bits than access type" );

	void const* const source = reinterpret_cast<uint8_t const*>( &mStorage ) + kBytesBeforeIndex;
	return Compressor<AccessType, kBitsAtIndex, kBitsOffsetIntoIndex>::Decompress( source );
}



template<size_t... fieldSizes>
template<size_t index, typename AccessTypeT>
inline void BitField<fieldSizes...>::WriteAt( AccessTypeT const& value )
{
	using AccessType = AccessTypeT;

	constexpr size_t kBytesBeforeIndex = GetBytesBeforeIndex<index>();
	constexpr size_t kBitsOffsetIntoIndex = GetBitsOffsetIntoIndex<index>();
	constexpr size_t kBitsAtIndex = GetBitsAtIndex<index>();
	static_assert( sizeof( AccessType ) * 8 >= kBitsAtIndex, "Field has more bits than access type" );

	void* const destination = reinterpret_cast<uint8_t*>( &mStorage ) + kBytesBeforeIndex;
	Compressor<AccessType, kBitsAtIndex, kBitsOffsetIntoIndex>::Compress( value, destination );
}



template<size_t... fieldSizes>
template<size_t index>
inline constexpr size_t BitField<fieldSizes...>::GetBytesBeforeIndex()
{
	constexpr size_t kBitsBeforeIndex = ListSum<size_t, FieldsBeforeIndex<index>>::value;
	constexpr size_t kBytesBeforeIndex = kBitsBeforeIndex / 8;
	return kBytesBeforeIndex;
}



template<size_t... fieldSizes>
template<size_t index>
inline constexpr size_t BitField<fieldSizes...>::GetBitsOffsetIntoIndex()
{
	constexpr size_t kBitsBeforeIndex = ListSum<size_t, FieldsBeforeIndex<index>>::value;
	constexpr size_t kBytesBeforeIndex = kBitsBeforeIndex / 8;
	constexpr size_t kBitsOffsetIntoIndex = kBitsBeforeIndex - ( kBytesBeforeIndex * 8 );
	return kBitsOffsetIntoIndex;
}



template<size_t... fieldSizes>
template<size_t index>
inline constexpr size_t BitField<fieldSizes...>::GetBitsAtIndex()
{
	static_assert( FieldsAtOrAfterIndex<index>::kNumValues > 0, "Index past number of fields" );
	using FirstFieldAfterIndex = typename FieldsAtOrAfterIndex<index>::template ByIndex<0>;
	constexpr size_t kBitsAtIndex = FirstFieldAfterIndex::value;
	return kBitsAtIndex;
}

///////////////////////////////////////////////////////////////////////////////

// Compressing/decompressing any type with 0 bits is always a nop
template<typename AccessTypeT, size_t storageOffsetBits>
struct Compressor<AccessTypeT, 0, storageOffsetBits>
{
	using AccessType = AccessTypeT;

	static void Compress( AccessType const&, void* )
	{
		// No bits, no operation
	}
	static AccessType Decompress( void const* )
	{
		// No bits, no operation
		return AccessType{};
	}
};

///////////////////////////////////////////////////////////////////////////////

// In the absence of a specialization for a custom type, the default compressor
//  will try to match against primitive types with intuitive bitwise behavior
template<typename AccessTypeT, size_t storageSizeBits, size_t storageOffsetBits>
struct Compressor
{
	using AccessType = AccessTypeT;
	static_assert( sizeof( AccessType ) * 8 >= storageSizeBits, "Storage has more bits than access type" );

	static void Compress( AccessType const& source, void* destination )
	{
		CompressImpl<AccessType>( source, destination );
	}
	static AccessType Decompress( void const* source )
	{
		return DecompressImpl<AccessType>( source );
	}

	// Store bits as big-endian, writing from right-to-left
	template<typename AccessType,
		typename rftl::enable_if<rftl::is_integral<AccessType>::value, int>::type = 0>
	static void CompressImpl( AccessType const& source, void* destination )
	{
#if defined( RF_PLATFORM_LITTLE_ENDIAN )
		AccessType const& sourceValue = source;
#elif defined( RF_PLATFORM_BIG_ENDIAN )
		AccessType const sourceValue = ReverseByteOrder( source );
#endif

		constexpr size_t kBytesNeededingModification = SnapHighest<size_t>( storageSizeBits, 8 ) / 8;
		constexpr int64_t kLeftMostByte = 0;
		constexpr int64_t kRightmostByte = kBytesNeededingModification - 1;

		constexpr size_t kExcessBytesToTrim = ( sizeof( AccessType ) * 8 ) - storageSizeBits;
		constexpr AccessType kIncomingSizeMask = static_cast<AccessType>( integer_unsigned_bitcast( GetAllBitsSet<AccessType>() ) >> kExcessBytesToTrim );
		AccessType incomingBitsFromRight = static_cast<AccessType>( sourceValue & kIncomingSizeMask );
		size_t bitsLeft = storageSizeBits;
		for( int64_t byteOffset = kRightmostByte; byteOffset >= kLeftMostByte; byteOffset-- )
		{
			uint8_t& storageByte = *( reinterpret_cast<uint8_t*>( destination ) + byteOffset );
			if( byteOffset == kLeftMostByte )
			{
				RF_ASSERT( bitsLeft <= 8 );
				size_t const bitsInByte = bitsLeft;
				size_t const leftShiftToMatchSize = 8 - bitsInByte;
				constexpr size_t rightShiftToMatchOffset = storageOffsetBits;
				uint8_t const incomingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( incomingBitsFromRight ) << leftShiftToMatchSize ) >> rightShiftToMatchOffset );
				uint8_t const incomingMask = static_cast<uint8_t>( static_cast<uint8_t>( GetAllBitsSet<uint8_t>() << leftShiftToMatchSize ) >> rightShiftToMatchOffset );
				uint8_t sharedBits = storageByte; // Load
				sharedBits &= ~incomingMask; // Clear
				sharedBits |= incomingBits; // Set
				storageByte = sharedBits; // Store
				incomingBitsFromRight >>= bitsInByte;
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft == 0 );
			}
			else if( byteOffset == kRightmostByte )
			{
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
				constexpr size_t bitsInByteMod = ( storageSizeBits + storageOffsetBits ) % 8;
				constexpr size_t bitsInByte = bitsInByteMod == 0 ? 8 : bitsInByteMod;
				constexpr size_t leftShiftToMatchSize = 8 - bitsInByte;
				constexpr size_t rightShiftToMatchOffset = 0;
				uint8_t const incomingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( incomingBitsFromRight ) << leftShiftToMatchSize ) >> rightShiftToMatchOffset );
				constexpr uint8_t incomingMask = static_cast<uint8_t>( static_cast<uint8_t>( GetAllBitsSet<uint8_t>() << leftShiftToMatchSize ) >> rightShiftToMatchOffset );
				uint8_t sharedBits = storageByte; // Load
				sharedBits &= ~incomingMask; // Clear
				sharedBits |= incomingBits; // Set
				storageByte = sharedBits; // Store
				if constexpr( bitsInByte == sizeof( AccessType ) * 8 )
				{
					// Shift would result in clear
					incomingBitsFromRight = 0;
				}
				else
				{
					static_assert( bitsInByte <= sizeof( AccessType ) * 8 );
					incomingBitsFromRight >>= bitsInByte;
				}
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
			}
			else
			{
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
				constexpr size_t bitsInByte = 8;
				size_t const leftShiftToMatchSize = 8 - bitsInByte;
				constexpr size_t rightShiftToMatchOffset = 0;
				uint8_t const incomingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( incomingBitsFromRight ) << leftShiftToMatchSize ) >> rightShiftToMatchOffset );
				uint8_t const incomingMask = ( static_cast<uint8_t>( 0xffu ) << leftShiftToMatchSize ) >> rightShiftToMatchOffset;
				uint8_t sharedBits = storageByte; // Load
				sharedBits &= ~incomingMask; // Clear
				sharedBits |= incomingBits; // Set
				storageByte = sharedBits; // Store
				if constexpr( bitsInByte == sizeof( AccessType ) * 8 )
				{
					// Shift would result in clear
					incomingBitsFromRight = 0;
				}
				else
				{
					static_assert( bitsInByte <= sizeof( AccessType ) * 8 );
					incomingBitsFromRight >>= bitsInByte;
				}
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
			}
		}
	}

	// Load bits as big-endian, reading from right-to-left
	template<typename AccessType,
		typename rftl::enable_if<rftl::is_integral<AccessType>::value, int>::type = 0>
	static AccessType DecompressImpl( void const* source )
	{
		constexpr size_t kBytesNeededingReads = SnapHighest<size_t>( storageSizeBits + storageOffsetBits, 8 ) / 8;
		constexpr int64_t kLeftMostByte = 0;
		constexpr int64_t kRightmostByte = kBytesNeededingReads - 1;

		AccessType destination = 0;

		size_t bitsLeft = storageSizeBits;
		for( int64_t byteOffset = kLeftMostByte; byteOffset <= kRightmostByte; byteOffset++ )
		{
			uint8_t const& storageByte = *( reinterpret_cast<uint8_t const*>( source ) + byteOffset );
			if( byteOffset == kLeftMostByte )
			{
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
				constexpr size_t bitsInByte = math::Min<size_t>( storageSizeBits, 8 - storageOffsetBits );
				constexpr size_t rightShiftToMatchOffset = ( 8 - bitsInByte ) - storageOffsetBits;
				constexpr size_t leftShiftToMatchSize = bitsInByte;
				constexpr AccessType outgoingMask = static_cast<AccessType>( ~static_cast<AccessType>( integer_unsigned_bitcast( GetAllBitsSet<AccessType>() ) << leftShiftToMatchSize ) );
				uint8_t const outgoingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( storageByte ) >> rightShiftToMatchOffset ) & outgoingMask );
				if constexpr( leftShiftToMatchSize == sizeof( AccessType ) * 8 )
				{
					// Shift would result in clear
					destination = 0;
				}
				else
				{
					static_assert( leftShiftToMatchSize <= sizeof( AccessType ) * 8 );
					destination <<= leftShiftToMatchSize; // Shift
					destination &= ~outgoingMask; // Clear
				}
				destination |= outgoingBits; // Set
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft >= 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
			}
			else if( byteOffset == kRightmostByte )
			{
				RF_ASSERT( bitsLeft <= 8 );
				size_t const bitsInByte = bitsLeft;
				size_t const rightShiftToMatchOffset = 8 - bitsInByte;
				size_t const leftShiftToMatchSize = bitsInByte;
				AccessType const outgoingMask = static_cast<AccessType>( ~static_cast<AccessType>( integer_unsigned_bitcast( GetAllBitsSet<AccessType>() ) << leftShiftToMatchSize ) );
				uint8_t const outgoingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( storageByte ) >> rightShiftToMatchOffset ) & outgoingMask );
				destination <<= leftShiftToMatchSize; // Shift
				destination &= ~outgoingMask; // Clear
				destination |= outgoingBits; // Set
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft == 0 );
			}
			else
			{
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
				constexpr size_t bitsInByte = 8;
				constexpr size_t rightShiftToMatchOffset = 0;
				constexpr size_t leftShiftToMatchSize = bitsInByte;
				constexpr AccessType outgoingMask = static_cast<AccessType>( ~static_cast<AccessType>( integer_unsigned_bitcast( GetAllBitsSet<AccessType>() ) << leftShiftToMatchSize ) );
				uint8_t const outgoingBits = static_cast<uint8_t>( static_cast<uint8_t>( static_cast<uint8_t>( storageByte ) >> rightShiftToMatchOffset ) & outgoingMask );
				if constexpr( leftShiftToMatchSize == sizeof( AccessType ) * 8 )
				{
					// Shift would result in clear
					destination = 0;
				}
				else
				{
					static_assert( leftShiftToMatchSize <= sizeof( AccessType ) * 8 );
					destination <<= leftShiftToMatchSize; // Shift
					destination &= ~outgoingMask; // Clear
				}
				destination |= outgoingBits; // Set
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
			}
		}

		return destination;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
