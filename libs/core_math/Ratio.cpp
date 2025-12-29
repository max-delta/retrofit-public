#include "stdafx.h"
#include "Ratio.h"

#include "core_math/math_bits.h"
#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"
#include "core_math/Lerp.h"
#include "core_math/Rand.h"

#include "core/meta/IntegerPromotion.h"

#include "rftl/extension/integer_literals.h"
#include "rftl/numeric"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// These should be larger than the largest supported storage components, to
//  ensure that multiplication math done with them doesn't need to check for
//  overflows
using SafeComponent = uint32_t;
using SafePair = rftl::pair<SafeComponent, SafeComponent>;

struct SafeCommon
{
	SafeComponent mLeft = 0;
	SafeComponent mRight = 0;
	SafeComponent mDenominator = 0;
};



template<typename StorageT, typename InterfaceT>
static constexpr InterfaceT MaxRepresentable()
{
	static_assert( sizeof( StorageT ) == sizeof( InterfaceT ) * 2 );
	return rftl::numeric_limits<InterfaceT>::max();
}
template<>
constexpr uint8_t MaxRepresentable<uint8_t, uint8_t>()
{
	return 0b00001111u;
}



template<typename StorageT, typename InterfaceT>
static StorageT Reduce( InterfaceT in )
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
static StorageT Store( InterfaceT numer, InterfaceT denom )
{
	StorageT const upper = Reduce<StorageT>( numer );
	StorageT const lower = Reduce<StorageT>( denom );
	static constexpr size_t kAvailableBits = sizeof( StorageT ) * 8;
	static constexpr size_t kShiftBits = kAvailableBits / 2;
	return angry_cast<StorageT>( ( upper << kShiftBits ) | lower );
}



template<typename StorageT, typename InterfaceT, bool UpperT>
static InterfaceT LoadHalf( StorageT storage )
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
static SafePair UpSizeComponents( RatioT in )
{
	// This code assumes we can just up-size the input, to drastically simplify
	//  the logic of things that could potentially have performed an overflow,
	//  at almost zero extra cost (just sign-extend the registers)
	static_assert( sizeof( SafeComponent ) > sizeof( typename RatioT::InterfaceType ) );
	typename RatioT::Pair const pair = in.GetAsPair();
	return { pair.first, pair.second };
}



template<typename RatioT>
static SafePair GetComparables( RatioT lhs, RatioT rhs )
{
	// Transformation / shuffling:
	//  a/b <=> c/d
	//  a*d/b <=> c
	//  a*d <=> c*b
	// NOTE: The act of multiplying could overflow, which is why we're jumping
	//  up to the next integer size
	SafePair const l = UpSizeComponents( lhs );
	SafePair const r = UpSizeComponents( rhs );
	return {
		l.first * r.second,
		r.first * l.second };
}



template<typename RatioT>
static SafeCommon MakeCommonDenominator( RatioT lhs, RatioT rhs )
{
	SafePair const l = UpSizeComponents( lhs );
	SafePair const r = UpSizeComponents( rhs );
	return SafeCommon{
		.mLeft = l.first * r.second,
		.mRight = r.first * l.second,
		.mDenominator = l.second * r.second };
}



static SafePair SafeSimplify( SafeComponent numerator, SafeComponent denominator )
{
	if( denominator == 0 )
	{
		return {};
	}

	SafeComponent const gcd = rftl::gcd( numerator, denominator );
	return {
		numerator / gcd,
		denominator / gcd };
}



template<typename RatioT>
static RatioT CreateFromSafe( SafeComponent numerator, SafeComponent denominator )
{
	static constexpr typename RatioT::InterfaceType kMax =
		MaxRepresentable<typename RatioT::StorageType, typename RatioT::InterfaceType>();
	if( numerator > kMax || denominator > kMax )
	{
		RF_DBGFAIL_MSG( "Ratio overflow" );
		return RatioT();
	}

	// NOTE: Integer casts in this case are theoretically overly paranoid
	return RatioT(
		math::integer_cast<typename RatioT::InterfaceType>( numerator ),
		math::integer_cast<typename RatioT::InterfaceType>( denominator ) );
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
	InterfaceType const denom = Denominator();
	if( denom == 0 )
	{
		[[unlikely]];
		return rftl::numeric_limits<float>::signaling_NaN();
	}
	return ( 1.f * Numerator() ) / denom;
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::InterfaceType Ratio<StorageT, InterfaceT>::GetAsIntegerViaTruncation() const
{
	InterfaceType const denom = Denominator();

	if( denom == 0 )
	{
		RF_DBGFAIL_MSG( "Divide by zero" );
		return 0;
	}

	return angry_cast<InterfaceType>( Numerator() / denom );
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
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::Simplify() const
{
	Pair const pair = GetAsPair();
	if( pair.second == 0 )
	{
		return Ratio();
	}

	InterfaceType const gcd = rftl::gcd( pair.first, pair.second );
	return Ratio(
		angry_cast<InterfaceType>( pair.first / gcd ),
		angry_cast<InterfaceType>( pair.second / gcd ) );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>::BitCounts Ratio<StorageT, InterfaceT>::GetBitsRequiredToRepresent() const
{
	return {
		math::GetOnesIndexOfHighestBit( Numerator() ),
		math::GetOnesIndexOfHighestBit( Denominator() ) };
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::TryLossyCompress( size_t mostDesiredBitsInEitherComponent ) const
{
	RF_ASSERT( mostDesiredBitsInEitherComponent > 0 );
	static constexpr size_t kBitsAvailable = GetOnesIndexOfHighestBit( details::MaxRepresentable<StorageType, InterfaceType>() );
	RF_ASSERT( mostDesiredBitsInEitherComponent < kBitsAvailable );

	if( IsValid() == false )
	{
		// If it's invalid, atleast make sure it's the invalid with the least
		//  number of bits in it
		return Ratio();
	}

	using Bits = rftl::pair<size_t, size_t>;
	static constexpr auto most = []( Bits const& in ) -> size_t
	{
		return Max( in.first, in.second );
	};
	static constexpr auto least = []( Bits const& in ) -> size_t
	{
		return Min( in.first, in.second );
	};

	// Simplify before anything else
	Ratio retVal = Simplify();
	Bits const bitsAtStart = retVal.GetBitsRequiredToRepresent();
	size_t const mostBitsAtStart = most( bitsAtStart );
	if( mostBitsAtStart <= mostDesiredBitsInEitherComponent )
	{
		// Simplifying was enough!
		return retVal;
	}

	size_t const leastBitsAtStart = least( bitsAtStart );
	if( leastBitsAtStart == 0 )
	{
		// This should only occur for the simplified ratio 0/1, which is the
		//  most compressible form of zero
		RF_ASSERT( bitsAtStart.first == 0 );
		RF_ASSERT( bitsAtStart.second == 1 );
		return retVal;
	}

	// How many bits CAN we lop off?
	size_t const maxCompression = leastBitsAtStart - 1;
	if( maxCompression == 0 )
	{
		// Can't compress via shift
		RF_ASSERT( bitsAtStart.first == 1 || bitsAtStart.second == 1 );
		return retVal;
	}

	// How many bits do we WANT to lop off?
	size_t const desideredCompression = mostBitsAtStart - mostDesiredBitsInEitherComponent;

	// How many bits WILL we lop off?
	size_t const chosenCompression = Min( desideredCompression, maxCompression );
	RF_ASSERT( chosenCompression > 0 );

	// Lop off bits
	retVal >>= chosenCompression;

	// And then simplify AGAIN in case that unlocked more compression
	retVal = retVal.Simplify();

	return retVal;
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
	details::SafePair const comparables = details::GetComparables( *this, rhs );
	return comparables.first < comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator<=( Ratio rhs ) const
{
	details::SafePair const comparables = details::GetComparables( *this, rhs );
	return comparables.first <= comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator==( Ratio rhs ) const
{
	details::SafePair const comparables = details::GetComparables( *this, rhs );
	return comparables.first == comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>=( Ratio rhs ) const
{
	details::SafePair const comparables = details::GetComparables( *this, rhs );
	return comparables.first >= comparables.second;
}



template<typename StorageT, typename InterfaceT>
bool Ratio<StorageT, InterfaceT>::operator>( Ratio rhs ) const
{
	details::SafePair const comparables = details::GetComparables( *this, rhs );
	return comparables.first > comparables.second;
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::operator+( Ratio rhs ) const
{
	details::SafeCommon const common = details::MakeCommonDenominator( *this, rhs );
	details::SafePair const result =
		details::SafeSimplify(
			common.mLeft + common.mRight,
			common.mDenominator );
	return details::CreateFromSafe<Ratio>( result.first, result.second );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::operator-( Ratio rhs ) const
{
	details::SafeCommon const common = details::MakeCommonDenominator( *this, rhs );
	if( common.mLeft < common.mRight )
	{
		RF_DBGFAIL_MSG( "Ratio overflow from subtraction" );
		return Ratio();
	}

	details::SafePair const result =
		details::SafeSimplify(
			common.mLeft - common.mRight,
			common.mDenominator );
	return details::CreateFromSafe<Ratio>( result.first, result.second );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::operator*( Ratio rhs ) const
{
	details::SafePair const l = details::UpSizeComponents( *this );
	details::SafePair const r = details::UpSizeComponents( rhs );
	details::SafePair const result =
		details::SafeSimplify(
			l.first * r.first,
			l.second * r.second );
	return details::CreateFromSafe<Ratio>( result.first, result.second );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::operator/( Ratio rhs ) const
{
	details::SafePair const l = details::UpSizeComponents( *this );
	details::SafePair const r = details::UpSizeComponents( rhs );
	details::SafePair const result =
		details::SafeSimplify(
			l.first * r.second,
			l.second * r.first );
	return details::CreateFromSafe<Ratio>( result.first, result.second );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT> Ratio<StorageT, InterfaceT>::operator>>( size_t numBits ) const
{
	return Ratio(
		angry_cast<InterfaceT>( Numerator() >> numBits ),
		angry_cast<InterfaceT>( Denominator() >> numBits ) );
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>& Ratio<StorageT, InterfaceT>::operator+=( Ratio rhs )
{
	*this = *this + rhs;
	return *this;
}


template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>& Ratio<StorageT, InterfaceT>::operator-=( Ratio rhs )
{
	*this = *this - rhs;
	return *this;
}


template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>& Ratio<StorageT, InterfaceT>::operator*=( Ratio rhs )
{
	*this = *this * rhs;
	return *this;
}


template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>& Ratio<StorageT, InterfaceT>::operator/=( Ratio rhs )
{
	*this = *this / rhs;
	return *this;
}



template<typename StorageT, typename InterfaceT>
Ratio<StorageT, InterfaceT>& Ratio<StorageT, InterfaceT>::operator>>=( size_t numBits )
{
	*this = *this >> numBits;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}

template class RF::math::Ratio<uint8_t, uint8_t>;
template class RF::math::Ratio<uint16_t, uint8_t>;
template class RF::math::Ratio<uint32_t, uint16_t>;
