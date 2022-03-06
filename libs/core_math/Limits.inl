#pragma once
#include "Limits.h"

#include "core_math/SoftFloat.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_quiet_NaN, int>::type>
inline bool is_quiet_NaN( T const& val )
{
	static_assert( rftl::numeric_limits<T>::is_iec559, "Only IEEE-754 supported currently" );
	return SoftFloat<T>::IsQuietNaN( val );
}

template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_quiet_NaN == false, int>::type>
inline bool is_quiet_NaN( T const& val )
{
	return false;
}



template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_signaling_NaN, int>::type>
inline bool is_signaling_NaN( T const& val )
{
	static_assert( rftl::numeric_limits<T>::is_iec559, "Only IEEE-754 supported currently" );
	return SoftFloat<T>::IsSignalingNaN( val );
}

template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_signaling_NaN == false, int>::type>
inline bool is_signaling_NaN( T const& val )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
