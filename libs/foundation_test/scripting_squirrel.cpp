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
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &xElem ) != nullptr );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &xElem )->GetAs<SquirrelVM::Integer>() != nullptr );
	SquirrelVM::Element yElem = vm.GetGlobalVariable( "y" );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &yElem ) != nullptr );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &yElem )->GetAs<SquirrelVM::FloatingPoint>() != nullptr );
	SquirrelVM::Element zElem = vm.GetGlobalVariable( "z" );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &zElem ) != nullptr );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &zElem )->GetAs<SquirrelVM::Boolean>() != nullptr );
	SquirrelVM::Element sElem = vm.GetGlobalVariable( "s" );
	ASSERT_TRUE( rftl::get_if<SquirrelVM::String>( &sElem ) != nullptr );
	SquirrelVM::Element nElem = vm.GetGlobalVariable( "n" );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &nElem ) != nullptr );
	ASSERT_TRUE( rftl::get_if<reflect::Value>( &nElem )->GetAs<SquirrelVM::Pointer>() != nullptr );
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
