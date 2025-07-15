#include "stdafx.h"

#include "Logging/Logging.h"


RF_CLANG_IGNORE( "-Wunreachable-code" );

namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////
namespace details {
	struct TestLoggingContext
	{
	};
}

template<> void WriteContextString( details::TestLoggingContext const& context, Utf8LogContextBuffer& buffer )
{
	(void)context;
	buffer[0] = '\0';
}



TEST( Logging, MacroResolution )
{
	// Don't want to run, just want to test compilation
	if( false )
	{
		constexpr char const* RFCAT_TEST = "Test";
		constexpr uint64_t RFCAT_TEST_SEV_WHITELIST = static_cast<uint64_t>( -1 );

		RFLOGF_TRACE( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_DEBUG( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_INFO( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_WARNING( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_ERROR( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_CRITICAL( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_NOTIFY( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_TEST_AND_NOTIFY( false, nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_CUSTOM( nullptr, RFCAT_TEST, 1, "This is a test %p", 0x0 );
		RFLOGF_FATAL( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
	}
}



TEST( Logging, CustomContext )
{
	// Don't want to run, just want to test compilation
	if( false )
	{
		constexpr char const* RFCAT_TEST = "Test";
		constexpr uint64_t RFCAT_TEST_SEV_WHITELIST = static_cast<uint64_t>( -1 );

		details::TestLoggingContext context;

		RFLOGF_TRACE( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_DEBUG( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_INFO( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_WARNING( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_ERROR( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_CRITICAL( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_NOTIFY( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_TEST_AND_NOTIFY( false, context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOGF_CUSTOM( context, RFCAT_TEST, 1, "This is a test %p", 0x0 );
		RFLOGF_FATAL( context, RFCAT_TEST, "This is a test %p", 0x0 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
