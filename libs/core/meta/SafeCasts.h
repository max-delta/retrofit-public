#pragma once
#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Safe cast that just grows the type intentionally, such as when expanding an
//  integer to a wider type to start doing bit-twiddling with it
template<typename DST, typename SRC>
inline constexpr DST broaden_cast( SRC in )
{
	constexpr bool kBothIntegers = rftl::is_integral<DST>::value && rftl::is_integral<SRC>::value;
	static_assert( kBothIntegers );
	constexpr bool kSameSigns = rftl::is_signed<DST>::value == rftl::is_signed<SRC>::value;
	static_assert( kSameSigns );
	static_assert( sizeof( DST ) > sizeof( SRC ) );
	return in;
}

// Safe cast that just helps a class do a conversion that it could do
//  implicitly, but maybe in some parsing contexts won't, and need a nudge
template<typename DST, typename SRC>
inline constexpr DST explicit_cast( SRC const& in )
{
	constexpr bool kIncludesClasses = rftl::is_class<DST>::value || rftl::is_class<SRC>::value;
	static_assert( kIncludesClasses );
	return in;
}

///////////////////////////////////////////////////////////////////////////////
}
