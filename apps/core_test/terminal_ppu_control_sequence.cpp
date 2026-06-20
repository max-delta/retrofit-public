#include "stdafx.h"

#include "core_terminal/PPUControlSequence.h"

#include "rftl/extension/array_concat.h"
#include "rftl/extension/array_string.h"
#include "rftl/extension/integer_literals.h"


namespace RF::term::ppu {
///////////////////////////////////////////////////////////////////////////////

TEST( ControlSequence, ValidByte )
{
	// Can't be null
	ASSERT_FALSE( IsValidDirectiveByte( 0 ) );

	// Must live within ASCII
	for( uint16_t ch = 1; ch < 128; ch++ )
	{
		ASSERT_TRUE( IsValidDirectiveByte( static_cast<char>( ch ) ) );
	}

	// Can't look like UTF-8
	for( uint16_t ch = 128; ch < 256; ch++ )
	{
		ASSERT_FALSE( IsValidDirectiveByte( static_cast<char>( ch ) ) );
	}
}


TEST( ControlSequence, ClearAll )
{
	using namespace rftl::literals;

	static constexpr rftl::array const seq = rftl::array_concat( sequence ::ClearAllState(), "ab\0"_array );
	static constexpr rftl::string_view const str( seq.data() );

	ASSERT_EQ( DiscardControlSequence( str ), "ab" );

	PPUParseState state = { .mPalleteIndex = 1_u8, .mReserved = true };
	ASSERT_EQ( ConsumeControlSequence( state, str ), "ab" );
	PPUParseState const expected{};
	ASSERT_EQ( state, expected );
}



TEST( ControlSequence, SetPallete )
{
	using namespace rftl::literals;

	static constexpr rftl::array const seq = rftl::array_concat( sequence ::SetPallete( 2 ), "ab\0"_array );
	static constexpr rftl::string_view const str( seq.data() );

	ASSERT_EQ( DiscardControlSequence( str ), "ab" );

	PPUParseState state = { .mPalleteIndex = 1_u8, .mReserved = true };
	ASSERT_EQ( ConsumeControlSequence( state, str ), "ab" );
	PPUParseState const expected{ .mPalleteIndex = 2_u8, .mReserved = true };
	ASSERT_EQ( state, expected );
}


TEST( ControlSequence, ClearPallete )
{
	using namespace rftl::literals;

	static constexpr rftl::array const seq = rftl::array_concat( sequence ::ClearPallete(), "ab\0"_array );
	static constexpr rftl::string_view const str( seq.data() );

	ASSERT_EQ( DiscardControlSequence( str ), "ab" );

	PPUParseState state = { .mPalleteIndex = 1_u8, .mReserved = true };
	ASSERT_EQ( ConsumeControlSequence( state, str ), "ab" );
	PPUParseState const expected{ .mReserved = true };
	ASSERT_EQ( state, expected );
}

///////////////////////////////////////////////////////////////////////////////
}
