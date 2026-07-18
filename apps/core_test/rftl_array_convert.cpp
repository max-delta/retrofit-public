#include "stdafx.h"

#include "rftl/extension/array_convert.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ArrayConvert, Basics )
{
	struct OneWay
	{
		explicit constexpr OneWay( int val )
			: mVal( val ) {}
		OneWay& operator=( OneWay const& ) = delete;
		bool operator==( OneWay const& ) const = default;
		int const mVal;
	};

	static constexpr rftl::array<int, 3> kInt = { 1, 2, 3 };
	static constexpr rftl::array<OneWay, 3> kObj = { OneWay{ 1 }, OneWay{ 2 }, OneWay{ 3 } };

	static_assert(
		rftl::array_convert<OneWay>( kInt ) ==
		kObj );
}

///////////////////////////////////////////////////////////////////////////////
}
