#include "stdafx.h"

#include "CommandLine/ArgParse.h"
#include "CommandLine/ArgView.h"

#include "rftl/type_traits"


namespace RF::cli {
///////////////////////////////////////////////////////////////////////////////

TEST( ArgParse, Empty )
{
	static constexpr char const* kArgv[] = {
		nullptr,
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 0 );
}



TEST( ArgParse, EmptyTerm )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"--",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 0 );
}



TEST( ArgParse, FalseEmptyTerm )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"--",
		"--",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 1 );
	ASSERT_EQ( parsed.GetArguments().at( 0 ), "--" );
}



TEST( ArgParse, ArgOnly )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"aaa",
		"bbb",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 2 );
	ASSERT_EQ( parsed.GetArguments().at( 0 ), "aaa" );
	ASSERT_EQ( parsed.GetArguments().at( 1 ), "bbb" );
}



TEST( ArgParse, SimpleOps )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"--help",
		"-?",
		"--version",
		"-v",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 4 );
	ASSERT_TRUE( parsed.HasOption( "--help" ) );
	ASSERT_TRUE( parsed.HasOption( "-?" ) );
	ASSERT_TRUE( parsed.HasOption( "--version" ) );
	ASSERT_TRUE( parsed.HasOption( "-v" ) );
	ASSERT_EQ( parsed.GetOptionVals( "--help" ).size(), 0 );
	ASSERT_EQ( parsed.GetOptionVals( "-?" ).size(), 0 );
	ASSERT_EQ( parsed.GetOptionVals( "--version" ).size(), 0 );
	ASSERT_EQ( parsed.GetOptionVals( "-v" ).size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 0 );
}



TEST( ArgParse, ParamOps )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"-a",
		"3",
		"-b",
		"5",
		"7",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 2 );
	ASSERT_TRUE( parsed.HasOption( "-a" ) );
	ASSERT_TRUE( parsed.HasOption( "-b" ) );
	ASSERT_EQ( parsed.GetOptionVals( "-a" ).size(), 1 );
	ASSERT_EQ( parsed.GetOptionVals( "-a" ).at( 0 ), "3" );
	ASSERT_EQ( parsed.GetOptionVals( "-b" ).size(), 2 );
	ASSERT_EQ( parsed.GetOptionVals( "-b" ).at( 0 ), "5" );
	ASSERT_EQ( parsed.GetOptionVals( "-b" ).at( 1 ), "7" );
	ASSERT_EQ( parsed.GetArguments().size(), 0 );
}



TEST( ArgParse, Complex )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"arg0",
		"arg1",
		"-opt0",
		"opt0val0",
		"opt0val1",
		"-opt1",
		"--",
		"arg2",
		"-arg3",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 2 );
	ASSERT_TRUE( parsed.HasOption( "-opt0" ) );
	ASSERT_TRUE( parsed.HasOption( "-opt1" ) );
	ASSERT_EQ( parsed.GetOptionVals( "-opt0" ).size(), 2 );
	ASSERT_EQ( parsed.GetOptionVals( "-opt0" ).at( 0 ), "opt0val0" );
	ASSERT_EQ( parsed.GetOptionVals( "-opt0" ).at( 1 ), "opt0val1" );
	ASSERT_EQ( parsed.GetOptionVals( "-opt1" ).size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 4 );
	ASSERT_EQ( parsed.GetArguments().at( 0 ), "arg0" );
	ASSERT_EQ( parsed.GetArguments().at( 1 ), "arg1" );
	ASSERT_EQ( parsed.GetArguments().at( 2 ), "arg2" );
	ASSERT_EQ( parsed.GetArguments().at( 3 ), "-arg3" );
}



TEST( ArgParse, HyphenArg )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"-",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 0 );
	ASSERT_EQ( parsed.GetArguments().size(), 1 );
	ASSERT_EQ( parsed.GetArguments().at( 0 ), "-" );
}



TEST( ArgParse, HyphenOps )
{
	static constexpr char const* kArgv[] = {
		nullptr,
		"-a",
		"-",
	};
	static constexpr size_t kArgc = rftl::extent<decltype( kArgv )>::value;
	ArgParse const parsed( { kArgc, kArgv } );

	ASSERT_EQ( parsed.GetOptionNames().size(), 1 );
	ASSERT_TRUE( parsed.HasOption( "-a" ) );
	ASSERT_EQ( parsed.GetOptionVals( "-a" ).size(), 1 );
	ASSERT_EQ( parsed.GetOptionVals( "-a" ).at( 0 ), "-" );
	ASSERT_EQ( parsed.GetArguments().size(), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
