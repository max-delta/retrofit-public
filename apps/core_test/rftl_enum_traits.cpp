#include "stdafx.h"

#include "rftl/extension/enum_traits.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( EnumTraits, ConditionalUnderlyingUnscopedType )
{
	{
		struct NonIntegral
		{
		};
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<NonIntegral>::type,
				NonIntegral>::value,
			"Unexpected value" );
	}
	{
		RF_CPP23_TODO( "This should fail to coerce to an int due to being a scoped type, and is_scoped_enum should be used to fix that" );
		enum class EnumScoped : uint8_t
		{
			Invalid = 0
		};
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<EnumScoped>::type,
				uint8_t>::value,
			"Unexpected value" );
	}
	{
		enum Enum32 : uint32_t
		{
			Invalid = 0
		};
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<Enum32>::type,
				uint32_t>::value,
			"Unexpected value" );
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<uint32_t>::type,
				uint32_t>::value,
			"Unexpected value" );
	}
	{
		enum Enum64 : uint64_t
		{
			Invalid = 0
		};
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<Enum64>::type,
				uint64_t>::value,
			"Unexpected value" );
		static_assert(
			rftl::is_same<
				rftl::conditional_underlying_unscoped_type<uint64_t>::type,
				uint64_t>::value,
			"Unexpected value" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
