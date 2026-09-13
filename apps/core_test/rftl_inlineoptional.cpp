#include "stdafx.h"

#include "rftl/extension/inline_optional.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( InlineOptional, Basics )
{
	using Opt = rftl::inline_optional<uint8_t, 255>;

	static_assert( Opt().has_value() == false );
	static_assert( Opt( 0 ).has_value() );
	static_assert( Opt( rftl::in_place, static_cast<uint8_t>( 0u ) ).has_value() );

	// SUBTLE: There's an x-value version and a reference version that can be
	//  different in theory
	static constexpr Opt kZero = Opt( 0 );
	static constexpr Opt kNull = Opt();
	static_assert( Opt( 0 ).value() == 0 );
	static_assert( kZero.value() == 0 );
	static_assert( *Opt( 0 ) == 0 );
	static_assert( *kZero == 0 );
	static_assert( Opt( 0 ).value_or( 5 ) == 0 );
	static_assert( kZero.value_or( 5 ) == 0 );
	static_assert( Opt().value_or( 5 ) == 5 );
	static_assert( kNull.value_or( 5 ) == 5 );
}

///////////////////////////////////////////////////////////////////////////////
}
