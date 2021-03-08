#pragma once
#include "core/meta/ValueList.h"
#include "core_math/ListOperations.h"
#include "core_math/math_clamps.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// Compressors compress/decompress different types in a bit-oriented manner
// NOTE: See the default compressor handling integral types for an example of
//  how to implement a compressor for a more complex custom type
template<typename AccessTypeT, size_t storageSizeBits, size_t storageOffsetBits>
struct Compressor;



// Bitfields store data in bits in a platform-independent way with consistent
//  endianness and padding behaviors, in contrast to the platform-dependent
//  bitfield language feature specified in the C++ standard
template<size_t... fieldSizes>
class BitField
{
public:
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
	template<size_t index, typename AccessTypeT>
	AccessTypeT ReadAt() const;
	template<size_t index, typename AccessTypeT>
	void WriteAt( AccessTypeT const& value );

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
}

#include "BitField.inl"
