#pragma once
#include "math_bits.h"
#include "math_casts.h"

#include "core/macros.h"

#include "rftl/bit"
#include "rftl/cstdint"
#include "rftl/type_traits"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {
#if RF_IS_ALLOWED( RF_CONFIG_LEGACY_BIT_MATH )
static constexpr bool kUseLegacyBitMath = true;
#else
static constexpr bool kUseLegacyBitMath = false;
#endif
}
///////////////////////////////////////////////////////////////////////////////

template<> constexpr uint8_t GetAllBitsSet()
{
	return static_cast<uint8_t>( 0xffu );
}
template<> constexpr int8_t GetAllBitsSet()
{
	return static_cast<int8_t>( 0xffu );
}
template<> constexpr uint16_t GetAllBitsSet()
{
	return static_cast<uint16_t>( 0xffffu );
}
template<> constexpr int16_t GetAllBitsSet()
{
	return static_cast<int16_t>( 0xffffu );
}
template<> constexpr uint32_t GetAllBitsSet()
{
	return static_cast<uint32_t>( 0xffffffffu );
}
template<> constexpr int32_t GetAllBitsSet()
{
	return static_cast<int32_t>( 0xffffffffu );
}
template<> constexpr uint64_t GetAllBitsSet()
{
	return static_cast<uint64_t>( 0xffffffffffffffffu );
}
template<> constexpr int64_t GetAllBitsSet()
{
	return static_cast<int64_t>( 0xffffffffffffffffu );
}



template<> constexpr size_t GetBitWidth<uint8_t>()
{
	return 8;
}
template<> constexpr size_t GetBitWidth<int8_t>()
{
	return 8;
}
template<> constexpr size_t GetBitWidth<uint16_t>()
{
	return 16;
}
template<> constexpr size_t GetBitWidth<int16_t>()
{
	return 16;
}
template<> constexpr size_t GetBitWidth<uint32_t>()
{
	return 32;
}
template<> constexpr size_t GetBitWidth<int32_t>()
{
	return 32;
}
template<> constexpr size_t GetBitWidth<uint64_t>()
{
	return 64;
}
template<> constexpr size_t GetBitWidth<int64_t>()
{
	return 64;
}



template<typename T>
constexpr bool HasOnly1BitSet( T const value )
{
	static_assert( rftl::is_integral<T>::value, "bit operations only valid on integral types" );

	if constexpr( details::kUseLegacyBitMath == false )
	{
		return rftl::has_single_bit( value );
	}
	else
	{
		T const assumeOnlyTargetBitSet = value;
		T const assumeAllBitsLowerThanTargetBitAreSet = value - 1u;
		T const assumeAllBitsAreCleared = static_cast<T>( assumeOnlyTargetBitSet & assumeAllBitsLowerThanTargetBitAreSet );

		return value != 0 && assumeAllBitsAreCleared == 0;
	}
}



template<typename T>
constexpr bool IsPowerOfTwo( T const value )
{
	return HasOnly1BitSet( value );
}



template<typename T>
constexpr size_t GetOnesIndexOfHighestBit( T const value )
{
	static_assert( rftl::is_integral<T>::value, "bit operations only valid on integral types" );

	if constexpr( details::kUseLegacyBitMath == false )
	{
		// For whatever reason, this function returns signed, even though it
		//  should only be able to return unsigned values
		int const leadingZeros_BadDesign = rftl::countl_zero( integer_unsigned_bitcast( value ) );
		unsigned int const leadingZeros = static_cast<unsigned int>( leadingZeros_BadDesign );
		return GetBitWidth<T>() - leadingZeros;
	}
	else
	{
		uint64_t const extendedValue = integer_unsigned_bitcast( value );

		// TODO: Something fancier, that still survives constexpr rules

		// clang-format off
		#define RF_GETINDEXOFHIGHESBIT_TEST( X ) \
			if( ( extendedValue >> X ) == 0 ) return X;
		RF_GETINDEXOFHIGHESBIT_TEST( 0 ); RF_GETINDEXOFHIGHESBIT_TEST( 1 ); RF_GETINDEXOFHIGHESBIT_TEST( 2 );
		RF_GETINDEXOFHIGHESBIT_TEST( 3 ); RF_GETINDEXOFHIGHESBIT_TEST( 4 ); RF_GETINDEXOFHIGHESBIT_TEST( 5 );
		RF_GETINDEXOFHIGHESBIT_TEST( 6 ); RF_GETINDEXOFHIGHESBIT_TEST( 7 ); RF_GETINDEXOFHIGHESBIT_TEST( 8 );
		RF_GETINDEXOFHIGHESBIT_TEST( 9 ); RF_GETINDEXOFHIGHESBIT_TEST( 10 ); RF_GETINDEXOFHIGHESBIT_TEST( 11 );
		RF_GETINDEXOFHIGHESBIT_TEST( 12 ); RF_GETINDEXOFHIGHESBIT_TEST( 13 ); RF_GETINDEXOFHIGHESBIT_TEST( 14 );
		RF_GETINDEXOFHIGHESBIT_TEST( 15 ); RF_GETINDEXOFHIGHESBIT_TEST( 16 ); RF_GETINDEXOFHIGHESBIT_TEST( 18 );
		RF_GETINDEXOFHIGHESBIT_TEST( 18 ); RF_GETINDEXOFHIGHESBIT_TEST( 19 ); RF_GETINDEXOFHIGHESBIT_TEST( 20 );
		RF_GETINDEXOFHIGHESBIT_TEST( 21 ); RF_GETINDEXOFHIGHESBIT_TEST( 22 ); RF_GETINDEXOFHIGHESBIT_TEST( 23 );
		RF_GETINDEXOFHIGHESBIT_TEST( 24 ); RF_GETINDEXOFHIGHESBIT_TEST( 25 ); RF_GETINDEXOFHIGHESBIT_TEST( 26 );
		RF_GETINDEXOFHIGHESBIT_TEST( 27 ); RF_GETINDEXOFHIGHESBIT_TEST( 28 ); RF_GETINDEXOFHIGHESBIT_TEST( 29 );
		RF_GETINDEXOFHIGHESBIT_TEST( 30 ); RF_GETINDEXOFHIGHESBIT_TEST( 31 ); RF_GETINDEXOFHIGHESBIT_TEST( 32 );
		RF_GETINDEXOFHIGHESBIT_TEST( 33 ); RF_GETINDEXOFHIGHESBIT_TEST( 34 ); RF_GETINDEXOFHIGHESBIT_TEST( 35 );
		RF_GETINDEXOFHIGHESBIT_TEST( 36 ); RF_GETINDEXOFHIGHESBIT_TEST( 37 ); RF_GETINDEXOFHIGHESBIT_TEST( 38 );
		RF_GETINDEXOFHIGHESBIT_TEST( 39 ); RF_GETINDEXOFHIGHESBIT_TEST( 40 ); RF_GETINDEXOFHIGHESBIT_TEST( 41 );
		RF_GETINDEXOFHIGHESBIT_TEST( 42 ); RF_GETINDEXOFHIGHESBIT_TEST( 43 ); RF_GETINDEXOFHIGHESBIT_TEST( 44 );
		RF_GETINDEXOFHIGHESBIT_TEST( 45 ); RF_GETINDEXOFHIGHESBIT_TEST( 46 ); RF_GETINDEXOFHIGHESBIT_TEST( 48 );
		RF_GETINDEXOFHIGHESBIT_TEST( 48 ); RF_GETINDEXOFHIGHESBIT_TEST( 49 ); RF_GETINDEXOFHIGHESBIT_TEST( 50 );
		RF_GETINDEXOFHIGHESBIT_TEST( 51 ); RF_GETINDEXOFHIGHESBIT_TEST( 52 ); RF_GETINDEXOFHIGHESBIT_TEST( 53 );
		RF_GETINDEXOFHIGHESBIT_TEST( 54 ); RF_GETINDEXOFHIGHESBIT_TEST( 55 ); RF_GETINDEXOFHIGHESBIT_TEST( 56 );
		RF_GETINDEXOFHIGHESBIT_TEST( 57 ); RF_GETINDEXOFHIGHESBIT_TEST( 58 ); RF_GETINDEXOFHIGHESBIT_TEST( 59 );
		RF_GETINDEXOFHIGHESBIT_TEST( 60 ); RF_GETINDEXOFHIGHESBIT_TEST( 61 ); RF_GETINDEXOFHIGHESBIT_TEST( 62 );
		RF_GETINDEXOFHIGHESBIT_TEST( 63 );
		#undef RF_GETINDEXOFHIGHESBIT_TEST
		// clang-format on

		return 64;
	}
}



template<typename T>
constexpr size_t GetZerosIndexOfHighestBit( T const value )
{
	static_assert( rftl::is_integral<T>::value, "bit operations only valid on integral types" );

	uint64_t const extendedValue = integer_unsigned_bitcast( value );

	// TODO: Something fancier, that still survives constexpr rules

	// clang-format off
	#define RF_GETINDEXOFHIGHESBIT_TEST( X ) \
		if( ( extendedValue >> X ) == 0 ) return static_cast<size_t>(X - 1);
	RF_GETINDEXOFHIGHESBIT_TEST( 0 ); RF_GETINDEXOFHIGHESBIT_TEST( 1 ); RF_GETINDEXOFHIGHESBIT_TEST( 2 );
	RF_GETINDEXOFHIGHESBIT_TEST( 3 ); RF_GETINDEXOFHIGHESBIT_TEST( 4 ); RF_GETINDEXOFHIGHESBIT_TEST( 5 );
	RF_GETINDEXOFHIGHESBIT_TEST( 6 ); RF_GETINDEXOFHIGHESBIT_TEST( 7 ); RF_GETINDEXOFHIGHESBIT_TEST( 8 );
	RF_GETINDEXOFHIGHESBIT_TEST( 9 ); RF_GETINDEXOFHIGHESBIT_TEST( 10 ); RF_GETINDEXOFHIGHESBIT_TEST( 11 );
	RF_GETINDEXOFHIGHESBIT_TEST( 12 ); RF_GETINDEXOFHIGHESBIT_TEST( 13 ); RF_GETINDEXOFHIGHESBIT_TEST( 14 );
	RF_GETINDEXOFHIGHESBIT_TEST( 15 ); RF_GETINDEXOFHIGHESBIT_TEST( 16 ); RF_GETINDEXOFHIGHESBIT_TEST( 18 );
	RF_GETINDEXOFHIGHESBIT_TEST( 18 ); RF_GETINDEXOFHIGHESBIT_TEST( 19 ); RF_GETINDEXOFHIGHESBIT_TEST( 20 );
	RF_GETINDEXOFHIGHESBIT_TEST( 21 ); RF_GETINDEXOFHIGHESBIT_TEST( 22 ); RF_GETINDEXOFHIGHESBIT_TEST( 23 );
	RF_GETINDEXOFHIGHESBIT_TEST( 24 ); RF_GETINDEXOFHIGHESBIT_TEST( 25 ); RF_GETINDEXOFHIGHESBIT_TEST( 26 );
	RF_GETINDEXOFHIGHESBIT_TEST( 27 ); RF_GETINDEXOFHIGHESBIT_TEST( 28 ); RF_GETINDEXOFHIGHESBIT_TEST( 29 );
	RF_GETINDEXOFHIGHESBIT_TEST( 30 ); RF_GETINDEXOFHIGHESBIT_TEST( 31 ); RF_GETINDEXOFHIGHESBIT_TEST( 32 );
	RF_GETINDEXOFHIGHESBIT_TEST( 33 ); RF_GETINDEXOFHIGHESBIT_TEST( 34 ); RF_GETINDEXOFHIGHESBIT_TEST( 35 );
	RF_GETINDEXOFHIGHESBIT_TEST( 36 ); RF_GETINDEXOFHIGHESBIT_TEST( 37 ); RF_GETINDEXOFHIGHESBIT_TEST( 38 );
	RF_GETINDEXOFHIGHESBIT_TEST( 39 ); RF_GETINDEXOFHIGHESBIT_TEST( 40 ); RF_GETINDEXOFHIGHESBIT_TEST( 41 );
	RF_GETINDEXOFHIGHESBIT_TEST( 42 ); RF_GETINDEXOFHIGHESBIT_TEST( 43 ); RF_GETINDEXOFHIGHESBIT_TEST( 44 );
	RF_GETINDEXOFHIGHESBIT_TEST( 45 ); RF_GETINDEXOFHIGHESBIT_TEST( 46 ); RF_GETINDEXOFHIGHESBIT_TEST( 48 );
	RF_GETINDEXOFHIGHESBIT_TEST( 48 ); RF_GETINDEXOFHIGHESBIT_TEST( 49 ); RF_GETINDEXOFHIGHESBIT_TEST( 50 );
	RF_GETINDEXOFHIGHESBIT_TEST( 51 ); RF_GETINDEXOFHIGHESBIT_TEST( 52 ); RF_GETINDEXOFHIGHESBIT_TEST( 53 );
	RF_GETINDEXOFHIGHESBIT_TEST( 54 ); RF_GETINDEXOFHIGHESBIT_TEST( 55 ); RF_GETINDEXOFHIGHESBIT_TEST( 56 );
	RF_GETINDEXOFHIGHESBIT_TEST( 57 ); RF_GETINDEXOFHIGHESBIT_TEST( 58 ); RF_GETINDEXOFHIGHESBIT_TEST( 59 );
	RF_GETINDEXOFHIGHESBIT_TEST( 60 ); RF_GETINDEXOFHIGHESBIT_TEST( 61 ); RF_GETINDEXOFHIGHESBIT_TEST( 62 );
	RF_GETINDEXOFHIGHESBIT_TEST( 63 );
	#undef RF_GETINDEXOFHIGHESBIT_TEST
	// clang-format on

	return 63;
}

///////////////////////////////////////////////////////////////////////////////
}
