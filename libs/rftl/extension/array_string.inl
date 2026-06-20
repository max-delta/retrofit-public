#pragma once
#include "array_string.h"

namespace rftl::literals {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<size_t SizeT>
struct StringToArrayConverter
{
	static constexpr size_t kWithoutNull = SizeT - 1;
	rftl::array<char, kWithoutNull> mArray;

	consteval StringToArrayConverter( char const ( &str )[SizeT] )
	{
		if constexpr( kWithoutNull == 0 )
		{
			// This if check prevents a compiler warning about the loop being
			//  useless in this case
		}
		else
		{
			for( size_t i = 0; i < kWithoutNull; i++ )
			{
				mArray.at( i ) = str[i];
			}
		}
	}
};

template<size_t SizeT>
StringToArrayConverter( char const ( &str )[SizeT] ) -> StringToArrayConverter<SizeT>;

}
///////////////////////////////////////////////////////////////////////////////

template<details::StringToArrayConverter ConverterT>
inline consteval auto operator""_array()
{
	return ConverterT.mArray;
}

///////////////////////////////////////////////////////////////////////////////
}
