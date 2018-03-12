#include "stdafx.h"

#include "core_logging/LoggingRouter.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

namespace details {

static size_t gCount1, gCount2, gCount3, gCountAll;

void ClearGlobalCounts()
{
	gCount1 = gCount2 = gCount3 = gCountAll = 0;
}

TEST( Logging, Details_ClearGlobalCounts )
{
	gCount1 = gCount2 = gCount3 = gCountAll = 47;
	ClearGlobalCounts();
	ASSERT_EQ( details::gCount1, 0 );
	ASSERT_EQ( details::gCount2, 0 );
	ASSERT_EQ( details::gCount3, 0 );
	ASSERT_EQ( details::gCountAll, 0 );
}

void Handler1( LoggingRouter const&, LogEvent const&, va_list )
{
	gCount1++;
}
void Handler2( LoggingRouter const&, LogEvent const&, va_list )
{
	gCount2++;
}
void Handler3( LoggingRouter const&, LogEvent const&, va_list )
{
	gCount3++;
}
void HandlerAll( LoggingRouter const&, LogEvent const&, va_list )
{
	gCountAll++;
}

void SetupStandardRouter( LoggingRouter& router )
{
	router.SetSeverityString( 0x1, "1" );
	router.SetSeverityString( 0x2, "2" );
	router.SetSeverityString( 0x4, "3" );

	HandlerDefinition handlerDef;
	handlerDef.mSupportedSeverities = 0x1;
	handlerDef.mHandlerFunc = Handler1;
	router.RegisterHandler( handlerDef );
	handlerDef.mSupportedSeverities = 0x2;
	handlerDef.mHandlerFunc = Handler2;
	router.RegisterHandler( handlerDef );
	handlerDef.mSupportedSeverities = 0x4;
	handlerDef.mHandlerFunc = Handler3;
	router.RegisterHandler( handlerDef );
	handlerDef.mSupportedSeverities = 0x7;
	handlerDef.mHandlerFunc = HandlerAll;
	router.RegisterHandler( handlerDef );
}

}



TEST( Logging, UnsetNullLog )
{
	LoggingRouter router;
	router.Log( nullptr, "", 1, "" );
	router.Log( nullptr, "", 1, "" );
	router.Log( nullptr, "", 1, "" );
}



TEST( Logging, UnsetTrashVarArgsToNowhere )
{
	// Handlers will probably fall over, but router shouldn't, since it
	//  shouldn't be unpacking messages
	LoggingRouter router;
	router.Log( nullptr, "", 1, "%s%s%s", 0x1 );
	router.Log( nullptr, "", 1, "%s%s%s", 0x1 );
	router.Log( nullptr, "", 1, "%s%s%s", 0x1 );
}



TEST( Logging, Registration )
{
	LoggingRouter router;

	struct Invalid
	{
		static void InvalidFunc( LoggingRouter const&, LogEvent const&, va_list )
		{
			GTEST_FAIL();
		}
	};

	HandlerDefinition handlerDef;
	handlerDef.mSupportedSeverities = 0x1;
	handlerDef.mHandlerFunc = Invalid::InvalidFunc;

	HandlerID const id1 = router.RegisterHandler( handlerDef );
	ASSERT_NE( id1, kInvalidHandlerID );
	HandlerID const id2 = router.RegisterHandler( handlerDef );
	ASSERT_NE( id2, kInvalidHandlerID );
	ASSERT_NE( id1, id2 );

	router.UnregisterHandler( id1 );
	router.UnregisterHandler( id2 );

	// This should have no effect since the handlers should be unregistered
	router.Log( nullptr, "", 1, "%s%s%s", 0x1 );
}



TEST( Logging, Basics )
{
	LoggingRouter router;
	details::SetupStandardRouter( router );

	details::ClearGlobalCounts();
	router.Log( nullptr, "", 1, "%s%s%s", 0x1 );
	ASSERT_EQ( details::gCount1, 1 );
	ASSERT_EQ( details::gCount2, 0 );
	ASSERT_EQ( details::gCount3, 0 );
	ASSERT_EQ( details::gCountAll, 1 );

	details::ClearGlobalCounts();
	router.Log( nullptr, "", 2, "%s%s%s", 0x1 );
	ASSERT_EQ( details::gCount1, 0 );
	ASSERT_EQ( details::gCount2, 1 );
	ASSERT_EQ( details::gCount3, 0 );
	ASSERT_EQ( details::gCountAll, 1 );

	details::ClearGlobalCounts();
	router.Log( nullptr, "", 4, "%s%s%s", 0x1 );
	ASSERT_EQ( details::gCount1, 0 );
	ASSERT_EQ( details::gCount2, 0 );
	ASSERT_EQ( details::gCount3, 1 );
	ASSERT_EQ( details::gCountAll, 1 );
}



TEST( Logging, GetStringFromCategoryKey )
{
	LoggingRouter router;

	constexpr CategoryKey key = "literal";
	char const* string = router.GetStringFromCategoryKey( key );
	ASSERT_EQ( string, key );
}



TEST( Logging, SeverityStrings )
{
	LoggingRouter router;
	details::SetupStandardRouter( router );

	LoggingRouter::SeverityStrings severityStrings = router.GetSeverityStrings();
	ASSERT_TRUE( severityStrings.at( 0 ) != nullptr );
	ASSERT_TRUE( severityStrings.at( 1 ) != nullptr );
	ASSERT_TRUE( severityStrings.at( 2 ) != nullptr );
	for( size_t i = 3; i < severityStrings.size(); i++ )
	{
		ASSERT_TRUE( severityStrings.at( i ) == nullptr );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
