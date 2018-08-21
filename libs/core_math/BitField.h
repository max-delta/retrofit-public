#pragma once
#include "core/compiler.h"
#include "core/meta/ValueList.h"
#include "core_math/ListOperations.h"
#include "core_math/math_clamps.h"
#include "core_math/math_bits.h"
#include "core_math/math_bytes.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename AccessTypeT, size_t storageSizeBits, size_t storageOffsetBits>
struct Compressor;



template<typename AccessTypeT, size_t... fieldSizes>
class BitField
{
public:
	using AccessType = AccessTypeT;
	using FieldSizes = ValueList<size_t, fieldSizes...>;
	template<size_t index>
	using FieldsBeforeIndex = typename FieldSizes::template Split<index>::former;
	template<size_t index>
	using FieldsAtOrAfterIndex = typename FieldSizes::template Split<index>::latter;
	static constexpr size_t kNumFields = FieldSizes::kNumValues;
	static constexpr size_t kMinimumBitsStorage = 1;
	static constexpr size_t kRequiredBitsStorage = ListSum<size_t, FieldSizes>::value;
	static constexpr size_t kActualBitsStorage = SnapHighest<size_t>( Max<size_t>( kRequiredBitsStorage, kMinimumBitsStorage ), 8 );
	static constexpr size_t kActualBytesStorage = kActualBitsStorage / 8;


public:
	template<size_t index>
	AccessType ReadAt() const;
	template<size_t index>
	void WriteAt( AccessType const& value );

	void const* Data() const;


private:
	template<size_t index>
	static constexpr size_t GetBytesBeforeIndex();
	template<size_t index>
	static constexpr size_t GetBitsOffsetIntoIndex();
	template<size_t index>
	static constexpr size_t GetBitsAtIndex();


private:
	char mStorage[kActualBytesStorage];
};

///////////////////////////////////////////////////////////////////////////////

template<typename AccessTypeT, size_t ...fieldSizes>
inline void const * BitField<AccessTypeT, fieldSizes...>::Data() const
{
	return &mStorage[0];
}



template<typename AccessTypeT, size_t ...fieldSizes>
template<size_t index>
inline typename BitField<AccessTypeT, fieldSizes...>::AccessType BitField<AccessTypeT, fieldSizes...>::ReadAt() const
{
	constexpr size_t kBytesBeforeIndex = GetBytesBeforeIndex<index>();
	constexpr size_t kBitsOffsetIntoIndex = GetBitsOffsetIntoIndex<index>();
	constexpr size_t kBitsAtIndex = GetBitsAtIndex<index>();
	static_assert( sizeof( AccessType ) * 8 >= kBitsAtIndex, "Field has more bits than access type" );

	void const* const source = reinterpret_cast<uint8_t const*>( &mStorage ) + kBytesBeforeIndex;
	return Compressor<AccessType, kBitsAtIndex, kBitsOffsetIntoIndex>::Decompress( source );
}



template<typename AccessTypeT, size_t ...fieldSizes>
template<size_t index>
inline void BitField<AccessTypeT, fieldSizes...>::WriteAt( AccessType const & value )
{
	constexpr size_t kBytesBeforeIndex = GetBytesBeforeIndex<index>();
	constexpr size_t kBitsOffsetIntoIndex = GetBitsOffsetIntoIndex<index>();
	constexpr size_t kBitsAtIndex = GetBitsAtIndex<index>();
	static_assert( sizeof( AccessType ) * 8 >= kBitsAtIndex, "Field has more bits than access type" );

	void* const destination = reinterpret_cast<uint8_t*>( &mStorage ) + kBytesBeforeIndex;
	Compressor<AccessType, kBitsAtIndex, kBitsOffsetIntoIndex>::Compress( value, destination );
}



template<typename AccessTypeT, size_t ...fieldSizes>
template<size_t index>
inline constexpr size_t BitField<AccessTypeT, fieldSizes...>::GetBytesBeforeIndex()
{
	constexpr size_t kBitsBeforeIndex = ListSum<size_t, FieldsBeforeIndex<index>>::value;
	constexpr size_t kBytesBeforeIndex = kBitsBeforeIndex / 8;
	return kBytesBeforeIndex;
}



template<typename AccessTypeT, size_t ...fieldSizes>
template<size_t index>
inline constexpr size_t BitField<AccessTypeT, fieldSizes...>::GetBitsOffsetIntoIndex()
{
	constexpr size_t kBitsBeforeIndex = ListSum<size_t, FieldsBeforeIndex<index>>::value;
	constexpr size_t kBytesBeforeIndex = kBitsBeforeIndex / 8;
	constexpr size_t kBitsOffsetIntoIndex = kBitsBeforeIndex - ( kBytesBeforeIndex * 8 );
	return kBitsOffsetIntoIndex;
}



template<typename AccessTypeT, size_t ...fieldSizes>
template<size_t index>
inline constexpr size_t BitField<AccessTypeT, fieldSizes...>::GetBitsAtIndex()
{
	static_assert( FieldsAtOrAfterIndex<index>::kNumValues > 0, "Index past number of fields" );
	using FirstFieldAfterIndex = typename FieldsAtOrAfterIndex<index>::template ByIndex<0>;
	constexpr size_t kBitsAtIndex = FirstFieldAfterIndex::value;
	return kBitsAtIndex;
}



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
		#if defined(RF_PLATFORM_LITTLE_ENDIAN)
		AccessType const& sourceValue = source;
		#elif defined(RF_PLATFORM_BIG_ENDIAN)
		AccessType const sourceValue = ReverseByteOrder( source );
		#endif

		constexpr size_t kBytesNeededingModification = SnapHighest<size_t>( storageSizeBits, 8 ) / 8;
		constexpr int64_t kLeftMostByte = 0;
		constexpr int64_t kRightmostByte = kBytesNeededingModification - 1;

		constexpr size_t kExcessBytesToTrim = ( sizeof( AccessType ) * 8 ) - storageSizeBits;
		constexpr AccessType kIncomingSizeMask = GetAllBitsSet<AccessType>() >> kExcessBytesToTrim;
		AccessType incomingBitsFromRight = sourceValue & kIncomingSizeMask;
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
				incomingBitsFromRight >>= bitsInByte;
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
				incomingBitsFromRight >>= bitsInByte;
				bitsLeft -= bitsInByte;
				RF_ASSERT( bitsLeft > 0 );
				RF_ASSERT( bitsLeft <= storageSizeBits );
			}
		}
	}
	template<typename AccessType,
		typename rftl::enable_if<rftl::is_integral<AccessType>::value, int>::type = 0>
	static AccessType DecompressImpl( void const* source )
	{
		static_assert( false, "TODO" );
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
