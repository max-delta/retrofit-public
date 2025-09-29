#include "stdafx.h"

#include "core/meta/Literal.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( Literal, Basics )
{
	// Should not cause compilation errors

	static constexpr LiteralRef<char, 5> kTest{ "Test" };
	static_assert( kTest[0] == 'T' );
	static_assert( kTest[1] == 'e' );
	static_assert( kTest[2] == 's' );
	static_assert( kTest[3] == 't' );
	static_assert( kTest[4] == '\0' );
	static constexpr char const( &kDecayTest )[5] = kTest;
	static_assert( kDecayTest[0] == 'T' );
	static_assert( kDecayTest[1] == 'e' );
	static_assert( kDecayTest[2] == 's' );
	static_assert( kDecayTest[3] == 't' );
	static_assert( kDecayTest[4] == '\0' );
	static constexpr char const* kFarDecayTest = kTest;
	static_assert( kFarDecayTest[0] == 'T' );
	static_assert( kFarDecayTest[1] == 'e' );
	static_assert( kFarDecayTest[2] == 's' );
	static_assert( kFarDecayTest[3] == 't' );
	static_assert( kFarDecayTest[4] == '\0' );

	static constexpr LiteralRef<char, 4> kAux{ "Aux" };
	static_assert( kAux[0] == 'A' );
	static_assert( kAux[1] == 'u' );
	static_assert( kAux[2] == 'x' );
	static_assert( kAux[3] == '\0' );
	static constexpr char const( &kDecayAux )[4] = kAux;
	static_assert( kDecayAux[0] == 'A' );
	static_assert( kDecayAux[1] == 'u' );
	static_assert( kDecayAux[2] == 'x' );
	static_assert( kDecayAux[3] == '\0' );
	static constexpr char const* kFarDecayAux = kAux;
	static_assert( kFarDecayAux[0] == 'A' );
	static_assert( kFarDecayAux[1] == 'u' );
	static_assert( kFarDecayAux[2] == 'x' );
	static_assert( kFarDecayAux[3] == '\0' );

	static constexpr ConcatLiterals<char, 8> kConcat = kTest + kAux;
	static_assert( kConcat.kResult[0] == 'T' );
	static_assert( kConcat.kResult[1] == 'e' );
	static_assert( kConcat.kResult[2] == 's' );
	static_assert( kConcat.kResult[3] == 't' );
	static_assert( kConcat.kResult[4] == 'A' );
	static_assert( kConcat.kResult[5] == 'u' );
	static_assert( kConcat.kResult[6] == 'x' );
	static_assert( kConcat.kResult[7] == '\0' );
	static constexpr char const( &kDecayConcat )[8] = kConcat;
	static_assert( kDecayConcat[0] == 'T' );
	static_assert( kDecayConcat[1] == 'e' );
	static_assert( kDecayConcat[2] == 's' );
	static_assert( kDecayConcat[3] == 't' );
	static_assert( kDecayConcat[4] == 'A' );
	static_assert( kDecayConcat[5] == 'u' );
	static_assert( kDecayConcat[6] == 'x' );
	static_assert( kDecayConcat[7] == '\0' );
	static constexpr char const* kFarDecayConcat = kConcat;
	static_assert( kFarDecayConcat[0] == 'T' );
	static_assert( kFarDecayConcat[1] == 'e' );
	static_assert( kFarDecayConcat[2] == 's' );
	static_assert( kFarDecayConcat[3] == 't' );
	static_assert( kFarDecayConcat[4] == 'A' );
	static_assert( kFarDecayConcat[5] == 'u' );
	static_assert( kFarDecayConcat[6] == 'x' );
	static_assert( kFarDecayConcat[7] == '\0' );

	static constexpr ConcatLiterals<char, 8> kAdd1 = Literal<char>() + "Test" + "Aux";
	static constexpr ConcatLiterals<char, 8> kAdd2 = Literal( "Test" ) + "Aux";
	( (void)kAdd1 );
	( (void)kAdd2 );

	static_assert( Literal( "A" )[0] == 'A' );
	static_assert( Literal( u"A" )[0] == u'A' );
	static_assert( Literal( U"A" )[0] == U'A' );
	static_assert( sizeof( Literal( "A" )[0] ) == 1 );
	static_assert( sizeof( Literal( u"A" )[0] ) == 2 );
	static_assert( sizeof( Literal( U"A" )[0] ) == 4 );
	static_assert( sizeof( ( Literal( "A" ) + "" )[0] ) == 1 );
	static_assert( sizeof( ( Literal( u"A" ) + u"" )[0] ) == 2 );
	static_assert( sizeof( ( Literal( U"A" ) + U"" )[0] ) == 4 );
}

///////////////////////////////////////////////////////////////////////////////
}
