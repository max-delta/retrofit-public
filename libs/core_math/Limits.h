#pragma once
#include "rftl/cstdint"
#include "rftl/limits"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// Test for quiet NaN
template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_quiet_NaN, int>::type = 0>
inline bool is_quiet_NaN( T const& val );
template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_quiet_NaN == false, int>::type = 0>
inline bool is_quiet_NaN( T const& val );

// Test for signalling NaN
template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_signaling_NaN, int>::type = 0>
inline bool is_signaling_NaN( T const& val );
template<typename T, typename rftl::enable_if<rftl::numeric_limits<T>::has_signaling_NaN == false, int>::type = 0>
inline bool is_signaling_NaN( T const& val );

///////////////////////////////////////////////////////////////////////////////
}

#include "Limits.inl"
