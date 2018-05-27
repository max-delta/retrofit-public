#include "stdafx.h"

#include "Scripting_squirrel/squirrel.h"


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////

TEST( Squirrel, EmptyVM )
{
	SquirrelVM vm;
}



TEST( Squirrel, GlobalInteger )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- 5;"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	reflect::Value const* const ref = rftl::get_if<reflect::Value>( &elem );
	ASSERT_NE( ref, nullptr );
	SquirrelVM::Integer const* const val = ref->GetAs<SquirrelVM::Integer>();
	ASSERT_NE( val, nullptr );
	ASSERT_EQ( *val, 5 );
}



TEST( Squirrel, GlobalFloat )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- 7.0;"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	reflect::Value const* const ref = rftl::get_if<reflect::Value>( &elem );
	ASSERT_NE( ref, nullptr );
	SquirrelVM::FloatingPoint const* const val = ref->GetAs<SquirrelVM::FloatingPoint>();
	ASSERT_NE( val, nullptr );
	ASSERT_EQ( *val, 7.f );
}



TEST( Squirrel, GlobalBoolean )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- true;"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	reflect::Value const* const ref = rftl::get_if<reflect::Value>( &elem );
	ASSERT_NE( ref, nullptr );
	SquirrelVM::Boolean const* const val = ref->GetAs<SquirrelVM::Boolean>();
	ASSERT_NE( val, nullptr );
	ASSERT_EQ( *val, true );
}



TEST( Squirrel, GlobalNull )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- null;"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	reflect::Value const* const ref = rftl::get_if<reflect::Value>( &elem );
	ASSERT_NE( ref, nullptr );
	SquirrelVM::Pointer const* const val = ref->GetAs<SquirrelVM::Pointer>();
	ASSERT_NE( val, nullptr );
	ASSERT_EQ( *val, nullptr );
}



TEST( Squirrel, GlobalString )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- \"STRING\";"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	SquirrelVM::String const* const val = rftl::get_if<SquirrelVM::String>( &elem );
	ASSERT_NE( val, nullptr );
	ASSERT_EQ( *val, "STRING" );
}



TEST( Squirrel, GlobalArray )
{
	SquirrelVM vm;
	constexpr char source[] =
		"x <- [\"first\", \"second\"];"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );
	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	SquirrelVM::ArrayTag const* const val = rftl::get_if<SquirrelVM::ArrayTag>( &elem );
	ASSERT_NE( val, nullptr );
	SquirrelVM::ElementArray elemArr = vm.GetGlobalVariableAsArray( "x" );
	{
		ASSERT_EQ( elemArr.size(), 2 );
		SquirrelVM::String const* const firstVal = rftl::get_if<SquirrelVM::String>( &elemArr[0] );
		ASSERT_NE( firstVal, nullptr );
		ASSERT_EQ( *firstVal, "first" );
		SquirrelVM::String const* const secondVal = rftl::get_if<SquirrelVM::String>( &elemArr[1] );
		ASSERT_NE( secondVal, nullptr );
		ASSERT_EQ( *secondVal, "second" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
