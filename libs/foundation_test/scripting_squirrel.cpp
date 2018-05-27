#include "stdafx.h"

#include "Scripting_squirrel/squirrel.h"


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////

TEST( Squirrel, EmptyVM )
{
	SquirrelVM vm;
}



TEST( Squirrel, LegacyTest )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- 5;"
		"y <- 7.0;"
		"z <- true;"
		"s <- \"STRING\";"
		"n <- null;"
		"a <- [\"first\", \"second\"];"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );

	SquirrelVM::Element xElem = vm.GetGlobalVariable( "x" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::Integer>( &xElem ) != nullptr );
	SquirrelVM::Element yElem = vm.GetGlobalVariable( "y" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::FloatingPoint>( &yElem ) != nullptr );
	SquirrelVM::Element zElem = vm.GetGlobalVariable( "z" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::Boolean>( &zElem ) != nullptr );
	SquirrelVM::Element sElem = vm.GetGlobalVariable( "s" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::String>( &sElem ) != nullptr );
	SquirrelVM::Element nElem = vm.GetGlobalVariable( "n" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::Null>( &nElem ) != nullptr );
	SquirrelVM::Element aElem = vm.GetGlobalVariable( "a" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::ArrayTag>( &aElem ) != nullptr );
	SquirrelVM::ElementArray aElemArr = vm.GetGlobalVariableAsArray( "a" );
	{
		ASSERT_TRUE( aElemArr.size() == 2 );
		ASSERT_TRUE( rftl::get_if<SquirrelVM::String>( &aElemArr[0] ) != nullptr );
		ASSERT_TRUE( rftl::get_if<SquirrelVM::String>( &aElemArr[1] ) != nullptr );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
