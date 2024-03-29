#include "stdafx.h"

#include "Scripting_squirrel/squirrel.h"


namespace RF::script {
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

	SquirrelVM::ElementArray const elemArr = vm.GetGlobalVariableAsArray( "x" );
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



TEST( Squirrel, GlobalClass )
{
	SquirrelVM vm;

	constexpr char source[] =
		"class C\n"
		"{\n"
		"  a = null;\n"
		"  b = null;\n"
		"  c = \"default\";\n"
		"}\n"
		"x <- C();\n"
		"x.a = \"first\";\n"
		"x.b = \"second\";\n"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );

	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
	ASSERT_NE( val, nullptr );

	SquirrelVM::ElementMap const elemMap = vm.GetGlobalVariableAsInstance( "x" );
	{
		ASSERT_EQ( elemMap.size(), 3 );

		SquirrelVM::String const firstIndex = "a";
		ASSERT_EQ( elemMap.count( firstIndex ), 1 );
		SquirrelVM::String const* const firstVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( firstIndex ) );
		ASSERT_NE( firstVal, nullptr );
		ASSERT_EQ( *firstVal, "first" );

		SquirrelVM::String const secondIndex = "b";
		ASSERT_EQ( elemMap.count( secondIndex ), 1 );
		SquirrelVM::String const* const secondVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( secondIndex ) );
		ASSERT_NE( secondVal, nullptr );
		ASSERT_EQ( *secondVal, "second" );

		SquirrelVM::String const defaultIndex = "c";
		ASSERT_EQ( elemMap.count( defaultIndex ), 1 );
		SquirrelVM::String const* const defaultVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( defaultIndex ) );
		ASSERT_NE( defaultVal, nullptr );
		ASSERT_EQ( *defaultVal, "default" );
	}
}



TEST( Squirrel, NestedEmpty )
{
	SquirrelVM vm;
	SquirrelVM::NestedTraversalPath const path = {};

	SquirrelVM::Element const elem = vm.GetNestedVariable( path );
	SquirrelVM::TableTag const* const rootTable = rftl::get_if<SquirrelVM::TableTag>( &elem );
	ASSERT_NE( rootTable, nullptr );

	SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
	ASSERT_TRUE( elemMap.empty() );

	SquirrelVM::ElementArray const elemArr = vm.GetNestedVariableAsArray( path );
	ASSERT_TRUE( elemArr.empty() );
}



TEST( Squirrel, NestedClass )
{
	SquirrelVM vm;

	constexpr char source[] =
		"class C\n"
		"{\n"
		"  a = null;\n"
		"  b = null;\n"
		"}\n"
		"x <- C();\n"
		"x.a = \"first\";\n"
		"x.b = C();\n"
		"x.b.a = \"second\";\n"
		"x.b.b = null;\n"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );

	{
		SquirrelVM::NestedTraversalPath const path = { "x" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
		ASSERT_NE( val, nullptr );

		SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
		{
			ASSERT_EQ( elemMap.size(), 2 );

			SquirrelVM::String const firstIndex = "a";
			ASSERT_EQ( elemMap.count( firstIndex ), 1 );
			SquirrelVM::String const* const firstVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( firstIndex ) );
			ASSERT_NE( firstVal, nullptr );
			ASSERT_EQ( *firstVal, "first" );

			SquirrelVM::String const secondIndex = "b";
			ASSERT_EQ( elemMap.count( secondIndex ), 1 );
			SquirrelVM::InstanceTag const* const secondVal = rftl::get_if<SquirrelVM::InstanceTag>( &elemMap.at( secondIndex ) );
			ASSERT_NE( secondVal, nullptr );
		}
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "a" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::String const* const val = rftl::get_if<SquirrelVM::String>( &elem );
		ASSERT_NE( val, nullptr );
		ASSERT_EQ( *val, "first" );
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "b" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
		ASSERT_NE( val, nullptr );

		SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
		{
			ASSERT_EQ( elemMap.size(), 2 );

			SquirrelVM::String const firstIndex = "a";
			ASSERT_EQ( elemMap.count( firstIndex ), 1 );
			SquirrelVM::String const* const firstVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( firstIndex ) );
			ASSERT_NE( firstVal, nullptr );
			ASSERT_EQ( *firstVal, "second" );

			SquirrelVM::String const secondIndex = "b";
			ASSERT_EQ( elemMap.count( secondIndex ), 1 );
			reflect::Value const* const secondVal = rftl::get_if<reflect::Value>( &elemMap.at( secondIndex ) );
			ASSERT_NE( secondVal, nullptr );
			SquirrelVM::Pointer const* const ptr = secondVal->GetAs<SquirrelVM::Pointer>();
			ASSERT_NE( ptr, nullptr );
			ASSERT_EQ( *ptr, nullptr );
		}
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "b", "a" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::String const* const val = rftl::get_if<SquirrelVM::String>( &elem );
		ASSERT_NE( val, nullptr );
		ASSERT_EQ( *val, "second" );
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "b", "b" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		reflect::Value const* const val = rftl::get_if<reflect::Value>( &elem );
		ASSERT_NE( val, nullptr );
		SquirrelVM::Pointer const* const ptr = val->GetAs<SquirrelVM::Pointer>();
		ASSERT_NE( ptr, nullptr );
		ASSERT_EQ( *ptr, nullptr );
	}
}



TEST( Squirrel, ArrayClasses )
{
	SquirrelVM vm;

	constexpr char source[] =
		"class C\n"
		"{\n"
		"  a = null;\n"
		"}\n"
		"x <- [C(), C()];\n"
		"x[0].a = \"first\";\n"
		"x[1].a = \"second\";\n"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );

	{
		SquirrelVM::NestedTraversalPath const path = { "x" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::ArrayTag const* const val = rftl::get_if<SquirrelVM::ArrayTag>( &elem );
		ASSERT_NE( val, nullptr );

		SquirrelVM::ElementArray const elemArr = vm.GetNestedVariableAsArray( path );
		{
			ASSERT_EQ( elemArr.size(), 2 );

			SquirrelVM::InstanceTag const* const firstVal = rftl::get_if<SquirrelVM::InstanceTag>( &elemArr[0] );
			ASSERT_NE( firstVal, nullptr );

			SquirrelVM::InstanceTag const* const secondVal = rftl::get_if<SquirrelVM::InstanceTag>( &elemArr[1] );
			ASSERT_NE( secondVal, nullptr );
		}
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "0" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
		ASSERT_NE( val, nullptr );

		SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
		{
			ASSERT_EQ( elemMap.size(), 1 );

			SquirrelVM::String const index = "a";
			ASSERT_EQ( elemMap.count( index ), 1 );
			SquirrelVM::String const* const strVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( index ) );
			ASSERT_NE( strVal, nullptr );
			ASSERT_EQ( *strVal, "first" );
		}
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "0", "a" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::String const* const val = rftl::get_if<SquirrelVM::String>( &elem );
		ASSERT_NE( val, nullptr );
		ASSERT_EQ( *val, "first" );
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "1" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
		ASSERT_NE( val, nullptr );

		SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
		{
			ASSERT_EQ( elemMap.size(), 1 );

			SquirrelVM::String const index = "a";
			ASSERT_EQ( elemMap.count( index ), 1 );
			SquirrelVM::String const* const strVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( index ) );
			ASSERT_NE( strVal, nullptr );
			ASSERT_EQ( *strVal, "second" );
		}
	}
	{
		SquirrelVM::NestedTraversalPath const path = { "x", "1", "a" };

		SquirrelVM::Element const elem = vm.GetNestedVariable( path );
		SquirrelVM::String const* const val = rftl::get_if<SquirrelVM::String>( &elem );
		ASSERT_NE( val, nullptr );
		ASSERT_EQ( *val, "second" );
	}
}



TEST( Squirrel, InjectSimpleStruct )
{
	SquirrelVM vm;

	static constexpr char const kClassName[] = "C";
	static constexpr char const* kMemberNames[] = { "a", "b" };
	bool const inject = vm.InjectSimpleStruct( kClassName, kMemberNames, rftl::extent<decltype( kMemberNames )>::value );
	ASSERT_TRUE( inject );

	constexpr char source[] =
		"x <- C();\n"
		"x.a = \"first\";\n"
		"x.b = \"second\";\n"
		"\n";
	bool const sourceAdd = vm.AddSourceFromBuffer( source );
	ASSERT_TRUE( sourceAdd );

	SquirrelVM::Element const elem = vm.GetGlobalVariable( "x" );
	SquirrelVM::InstanceTag const* const val = rftl::get_if<SquirrelVM::InstanceTag>( &elem );
	ASSERT_NE( val, nullptr );

	SquirrelVM::ElementMap const elemMap = vm.GetGlobalVariableAsInstance( "x" );
	{
		ASSERT_EQ( elemMap.size(), 3 );

		SquirrelVM::String const reservedIndex = SquirrelVM::kReservedClassNameMemberName;
		ASSERT_EQ( elemMap.count( reservedIndex ), 1 );
		SquirrelVM::String const* const reservedVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( reservedIndex ) );
		ASSERT_NE( reservedVal, nullptr );
		ASSERT_EQ( *reservedVal, kClassName );

		SquirrelVM::String const firstIndex = "a";
		ASSERT_EQ( elemMap.count( firstIndex ), 1 );
		SquirrelVM::String const* const firstVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( firstIndex ) );
		ASSERT_NE( firstVal, nullptr );
		ASSERT_EQ( *firstVal, "first" );

		SquirrelVM::String const secondIndex = "b";
		ASSERT_EQ( elemMap.count( secondIndex ), 1 );
		SquirrelVM::String const* const secondVal = rftl::get_if<SquirrelVM::String>( &elemMap.at( secondIndex ) );
		ASSERT_NE( secondVal, nullptr );
		ASSERT_EQ( *secondVal, "second" );
	}

	rftl::string const instanceClassName = vm.GetGlobalInstanceClassName( "x" );
	ASSERT_EQ( instanceClassName, kClassName );
}

///////////////////////////////////////////////////////////////////////////////
}
