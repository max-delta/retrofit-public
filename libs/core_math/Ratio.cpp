#include "stdafx.h"
#include "Ratio.h"

#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"
#include "core_math/Lerp.h"
#include "core_math/Rand.h"

#include "core/meta/IntegerPromotion.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename StorageT, typename InterfaceT>
StorageT Reduce( InterfaceT in )
{
	static_assert( sizeof( StorageT ) == sizeof( InterfaceT ) * 2 );
	return in;
}
template<>
uint8_t Reduce<uint8_t, uint8_t>( uint8_t in )
{
	RF_ASSERT( in <= 0b00001111u );
	return in & 0b00001111u;
}



template<typename StorageT, typename InterfaceT>
StorageT Store( InterfaceT numer, InterfaceT denom )
{
	StorageT const upper = Reduce<StorageT>( numer );
	StorageT const lower = Reduce<StorageT>( denom );
	static constexpr size_t kAvailableBits = sizeof( StorageT ) * 8;
	static constexpr size_t kShiftBits = kAvailableBits / 2;
	return angry_cast<StorageT>( ( upper << kShiftBits ) | lower );
}



template<typename StorageT, typename InterfaceT, bool UpperT>
InterfaceT LoadHalf( StorageT storage )
{
	static constexpr size_t kAvailableBits = sizeof( StorageT ) * 8;
	static constexpr size_t kShiftBits = kAvailableBits / 2;
	static constexpr StorageT kTotalMask = angry_cast<StorageT>( ~StorageT{} );
	static constexpr StorageT kLowerMask = kTotalMask >> kShiftBits;
	if constexpr( UpperT )
	{
		StorageT const upper = angry_cast<StorageT>( storage >> kShiftBits );
		return static_cast<InterfaceT>( upper );
	}
	else
	{
		StorageT const lower = angry_cast<StorageT>( storage & kLowerMask );
		return static_cast<InterfaceT>( lower );
	}
}


template<typename RatioT>
rftl::pair<uint32_t, uint32_t> GetComparables( RatioT lhs, RatioT rhs )
{
	// This code assumes we can just up-size the input, to drastically simplify
	//  the logic at almost zero extra cost (just sign-extend the registers)
	static_assert( sizeof( uint32_t ) > sizeof( typename RatioT::InterfaceType ) );

	// Transformation / shuffling:
	//  a/b <=> c/d
	//  a*d/b <=> c
	//  a*d <=> c*b
	// NOTE: The act of multiplying could overflow, which is why we're jumping
	//  up to the next integer size
	typename RatioT::Pair const l = lhs.GetAsPair();
	typename RatioT::Pair const r = rhs.GetAsPair();
	rftl::pair<uint32_t, uint32_t> retVal = { l.first, r.first };
	retVal.first *= r.second;
	retVal.second *= l.second;
	return retVal;
}


}
///////////////////////////////////////////////////////////////////////////////

template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::Ratio() = default;



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::Ratio( InterfaceType numerator, InterfaceType denominator )
	: mStorage( details::Store<StorageType>( numerator, denominator ) )
{
	//
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::Ratio( Pair pair )
	: Ratio( pair.first, pair.second )
{
	//
}



template<typename StorageT, typename InterfaceT>
typename Ratio<StorageT, InterfaceT>::Pair Ratio<StorageT, InterfaceT>::GetAsPair() const
{
	return { Numerator(), Denominator() };
}



template<typename StorageT, typename InterfaceT>
float Ratio<StorageT, InterfaceT>::GetAsFloat() const
{
	return ( 1.f * Numerator() ) / Denominator();
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::InterfaceType Ratio<StorageT, InterfaceT>::Numerator() const
{
	return details::LoadHalf<StorageT, InterfaceType, true>( mStorage );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::InterfaceType Ratio<StorageT, InterfaceT>::Denominator() const
{
	return details::LoadHalf<StorageT, InterfaceType, false>( mStorage );
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::IsValid() const
{
	return Denominator() != 0;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::IsIntegral() const
{
	InterfaceType const denom = Denominator();
	if( denom == 0 )
	{
		// Would divide by zero
		return false;
	}

	if( ( Numerator() % denom ) == 0 )
	{
		// Cleanly divisible
		return true;
	}

	return false;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator<( InterfaceT rhs ) const
{
	Ratio const rhsAsRatio( rhs, 1u );
	return *this < rhsAsRatio;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator<=( InterfaceT rhs ) const
{
	Ratio const rhsAsRatio( rhs, 1u );
	return *this <= rhsAsRatio;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator==( InterfaceT rhs ) const
{
	Ratio const rhsAsRatio( rhs, 1u );
	return *this == rhsAsRatio;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>=( InterfaceT rhs ) const
{
	Ratio const rhsAsRatio( rhs, 1u );
	return *this >= rhsAsRatio;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>( InterfaceT rhs ) const
{
	Ratio const rhsAsRatio( rhs, 1u );
	return *this > rhsAsRatio;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator<( Ratio rhs ) const
{
	rftl::pair<uint32_t, uint32_t> const comparables = details::GetComparables( *this, rhs );
	return comparables.first < comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator<=( Ratio rhs ) const
{
	rftl::pair<uint32_t, uint32_t> const comparables = details::GetComparables( *this, rhs );
	return comparables.first <= comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator==( Ratio rhs ) const
{
	rftl::pair<uint32_t, uint32_t> const comparables = details::GetComparables( *this, rhs );
	return comparables.first == comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>=( Ratio rhs ) const
{
	rftl::pair<uint32_t, uint32_t> const comparables = details::GetComparables( *this, rhs );
	return comparables.first >= comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>( Ratio rhs ) const
{
	rftl::pair<uint32_t, uint32_t> const comparables = details::GetComparables( *this, rhs );
	return comparables.first > comparables.second;
}

///////////////////////////////////////////////////////////////////////////////
}

template class RF::math::Ratio<uint8_t, uint8_t>;
template class RF::math::Ratio<uint16_t, uint8_t>;
template class RF::math::Ratio<uint32_t, uint16_t>;
