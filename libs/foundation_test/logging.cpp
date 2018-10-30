#include "stdafx.h"

#include "Logging/Logging.h"


namespace RF { namespace logging {
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

		RFLOG_TRACE( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_DEBUG( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_INFO( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_WARNING( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_ERROR( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_CRITICAL( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_NOTIFY( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_TEST_AND_NOTIFY( false, nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_CUSTOM( nullptr, RFCAT_TEST, 1, "This is a test %p", 0x0 );
		RFLOG_FATAL( nullptr, RFCAT_TEST, "This is a test %p", 0x0 );
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

		RFLOG_TRACE( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_DEBUG( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_INFO( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_WARNING( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_ERROR( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_CRITICAL( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_NOTIFY( context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_TEST_AND_NOTIFY( false, context, RFCAT_TEST, "This is a test %p", 0x0 );
		RFLOG_CUSTOM( context, RFCAT_TEST, 1, "This is a test %p", 0x0 );
		RFLOG_FATAL( context, RFCAT_TEST, "This is a test %p", 0x0 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
