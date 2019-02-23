#pragma once
#include "math_casts.h"

#include "core/macros.h"

#include "rftl/limits"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename DST, typename SRC>
constexpr bool could_overflow()
{
	constexpr DST maxDest = rftl::numeric_limits<DST>::max();
	return rftl::numeric_limits<SRC>::max() > maxDest;
}



template<typename DST, typename SRC>
constexpr bool could_drop_sign()
{
	return rftl::is_signed<SRC>::value == true && rftl::is_signed<DST>::value == false;
}



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_signed<SRC>::value && rftl::is_signed<DST>::value, bool>::type could_underflow()
{
	constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
	constexpr SRC lowestSrc = rftl::numeric_limits<SRC>::lowest();
	return lowestSrc < lowestDest;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_signed<SRC>::value && rftl::is_signed<DST>::value == false, bool>::type could_underflow()
{
	return true;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_signed<SRC>::value == false, bool>::type could_underflow()
{
	return false;
}



#ifdef RF_PLATFORM_MSVC
// MSVC is once again, unable to do anything

template<typename DST, typename SRC>
constexpr bool will_overflow( SRC const src )
{
	if( could_overflow<DST, SRC>() )
	{
		constexpr DST maxDest = rftl::numeric_limits<DST>::max();
		return src > maxDest;
	}
	return false;
}

template<typename DST, typename SRC>
constexpr bool will_drop_sign( SRC const src )
{
	if( could_drop_sign<DST, SRC>() )
	{
		return src < 0;
	}
	return false;
}

template<typename DST, typename SRC>
constexpr bool will_underflow( SRC const src )
{
	if( could_underflow<DST, SRC>() )
	{
		if( could_drop_sign<DST, SRC>() )
		{
			return will_drop_sign<DST, SRC>( src );
		}
		constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
		RF_ACK_CONSTEXPR_SIGN_MISMATCH;
		return src < lowestDest;
	}
	return false;
}

#else

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_overflow<DST, SRC>(), bool>::type will_overflow( SRC const src )
{
	constexpr DST maxDest = rftl::numeric_limits<DST>::max();
	return src > maxDest;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_overflow<DST, SRC>() == false, bool>::type will_overflow( SRC const src )
{
	return false;
}



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_drop_sign<DST, SRC>(), bool>::type will_drop_sign( SRC const src )
{
	return src < 0;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_drop_sign<DST, SRC>() == false, bool>::type will_drop_sign( SRC const src )
{
	return false;
}



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>() && could_drop_sign<DST, SRC>(), bool>::type will_underflow( SRC const src )
{
	return will_drop_sign<DST, SRC>( src );
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>() && could_drop_sign<DST, SRC>() == false, bool>::type will_underflow( SRC const src )
{
	constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
	return src < lowestDest;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>() == false, bool>::type will_underflow( SRC const src )
{
	return false;
}

#endif

}
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	DST retVal = integer_truncast<DST>( src );
	RF_ASSERT( retVal == static_cast<DST>( src ) );
	return retVal;
}



template<typename DST, typename SRC>
DST integer_truncast( SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_truncast only valid on integeral types" );

	if( details::will_overflow<DST, SRC>( src ) )
	{
		constexpr DST maxDest = rftl::numeric_limits<DST>::max();
		return maxDest;
	}

	if( details::will_underflow<DST, SRC>( src ) )
	{
		constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
		return lowestDest;
	}

	if( rftl::is_floating_point<SRC>::value )
	{
		if( rftl::is_same<SRC, long double>::value )
		{
			return static_cast<DST>( roundl( static_cast<long double>( src ) ) );
		}

		if( rftl::is_same<SRC, double>::value )
		{
			return static_cast<DST>( round( static_cast<double>( src ) ) );
		}

		return static_cast<DST>( roundf( static_cast<float>( src ) ) );
	}
	else
	{
		return static_cast<DST>( src );
	}
}



constexpr uint8_t integer_unsigned_cast( int8_t const src )
{
	return static_cast<uint8_t>( src );
}
constexpr uint16_t integer_unsigned_cast( int16_t const src )
{
	return static_cast<uint16_t>( src );
}
constexpr uint32_t integer_unsigned_cast( int32_t const src )
{
	return static_cast<uint32_t>( src );
}
constexpr uint64_t integer_unsigned_cast( int64_t const src )
{
	return static_cast<uint64_t>( src );
}
constexpr uint8_t integer_unsigned_cast( uint8_t const src )
{
	return src;
}
constexpr uint16_t integer_unsigned_cast( uint16_t const src )
{
	return src;
}
constexpr uint32_t integer_unsigned_cast( uint32_t const src )
{
	return src;
}
constexpr uint64_t integer_unsigned_cast( uint64_t const src )
{
	return src;
}



template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_integral<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return integer_cast<DST>( src );
}



template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_floating_point<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return static_cast<DST>( src );
}



template<typename SRC,
	typename rftl::enable_if<rftl::is_enum<SRC>::value, int>::type>
constexpr uint64_t enum_bitcast( SRC const src )
{
	return static_cast<uint64_t>(src);
}

///////////////////////////////////////////////////////////////////////////////
}}
