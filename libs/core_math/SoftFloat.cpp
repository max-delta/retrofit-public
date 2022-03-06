#include "stdafx.h"

#include "SoftFloat.h"

#include "core_math/BitField.h"


RF_CLANG_IGNORE( "-Wundefined-reinterpret-cast" );

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static_assert( rftl::numeric_limits<float>::is_iec559, "Only IEEE-754 supported currently" );
using BitFloat = BitField<1, 8, 23>;
static_assert( sizeof( BitFloat ) == 4, "Unexpected size" );
static_assert( sizeof( float ) == 4, "Unexpected size" );
static_assert( sizeof( SoftFloat<float>::Rep ) == 4, "Unexpected size" );
static BitFloat ConvertToSoft( float const& hardFloat )
{
	// HACK: This is probably wildly unsafe and hardware-dependent
	uint32_t const softUnswapped = *reinterpret_cast<uint32_t const*>( &hardFloat );
	uint32_t const softSwapped = math::FromPlatformToBigEndian( softUnswapped );
	return *reinterpret_cast<BitFloat const*>( &softSwapped );
}

}
///////////////////////////////////////////////////////////////////////////////

SoftFloat<float>::Rep SoftFloat<float>::SignBit( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	return soft.ReadAt<0, Rep>();
}



SoftFloat<float>::Rep SoftFloat<float>::ExponentBits( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	return soft.ReadAt<1, Rep>();
}



SoftFloat<float>::Rep SoftFloat<float>::FractionalBits( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	return soft.ReadAt<2, Rep>();
}



bool SoftFloat<float>::IsNegative( Value const& val )
{
	return SignBit( val ) == 0b1;
}



SoftFloat<float>::Exponent SoftFloat<float>::ExponentValue( Value const& val )
{
	Rep const exponentBits = ExponentBits( val );
	if( exponentBits == 0 )
	{
		// Is this the special zero case?
		if( FractionalBits( val ) == 0 )
		{
			return 0;
		}
	}
	return static_cast<int32_t>( exponentBits ) - 127;
}



bool SoftFloat<float>::IsDenormalized( Value const& val )
{
	if( ExponentBits( val ) == 0 )
	{
		if( FractionalBits( val ) != 0 )
		{
			return true;
		}
	}
	return false;
}



bool SoftFloat<float>::IsInfinity( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	bool const exponentAllOnes = soft.ReadAt<1, Rep>() == 0b1111'1111;
	if( exponentAllOnes )
	{
		Rep const fraction = soft.ReadAt<2, Rep>();
		bool const zeroFraction = fraction == 0;
		if( zeroFraction )
		{
			return true;
		}
	}
	return false;
}



bool SoftFloat<float>::IsNaN( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	bool const exponentAllOnes = soft.ReadAt<1, Rep>() == 0b1111'1111;
	if( exponentAllOnes )
	{
		Rep const fraction = soft.ReadAt<2, Rep>();
		bool const nonZeroFraction = fraction != 0;
		if( nonZeroFraction )
		{
			return true;
		}
	}
	return false;
}



bool SoftFloat<float>::IsQuietNaN( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	bool const exponentAllOnes = soft.ReadAt<1, Rep>() == 0b1111'1111;
	if( exponentAllOnes )
	{
		Rep const fraction = soft.ReadAt<2, Rep>();
		bool const nonZeroFraction = fraction != 0;
		if( nonZeroFraction )
		{
			Rep const mostSignificantBit = fraction & ( 1 << 22 );
			bool const isQuiet = mostSignificantBit == 0b1;
			return isQuiet;
		}
	}
	return false;
}



bool SoftFloat<float>::IsSignalingNaN( Value const& val )
{
	details::BitFloat const soft = details::ConvertToSoft( val );
	bool const exponentAllOnes = soft.ReadAt<1, Rep>() == 0b1111'1111;
	if( exponentAllOnes )
	{
		Rep const fraction = soft.ReadAt<2, Rep>();
		bool const nonZeroFraction = fraction != 0;
		if( nonZeroFraction )
		{
			Rep const mostSignificantBit = fraction & ( 1 << 22 );
			bool const isQuiet = mostSignificantBit == 0b1;
			return isQuiet == false;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
