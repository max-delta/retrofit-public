#include "stdafx.h"
#include "FallbackLogger.h"

#include "Logging/Logging.h"

#include "core_logging/LoggingRouter.h"

#include "rftl/extension/bounded_overwrite_iterator.h"
#include "rftl/atomic"
#include "rftl/limits"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

static rftl::atomic<HandlerID> sFallbackHandlerID = kInvalidHandlerID;

///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent<char8_t> const& event, rftl::format_args&& args )
{
	// C++20 APIs won't take Unicode, hope that ASCII is good enough
	rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

	constexpr size_t kBufSize = 512;
	rftl::array<char, kBufSize> messageBuffer;
	{
		rftl::bounded_forward_overwrite_iterator out( messageBuffer );
		out = rftl::vformat_to( out, legacyFormatString, args );
		*out = '\0';
		*messageBuffer.rbegin() = '\0';
	}

	rftl::array<char, kBufSize> outputBuffer;
	int const bytesParsed = snprintf( &outputBuffer[0], kBufSize, "FALLBACK_LOGGER>>[%s] %s", event.mCategoryKey, &messageBuffer[0] );
	*outputBuffer.rbegin() = '\0';

	puts( &outputBuffer[0] );
	static_assert( kBufSize <= static_cast<size_t>( rftl::numeric_limits<int>::max() ), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		puts( "FALLBACK_LOGGER>>TRUNCATED MESSAGE!" );
	}
}



void InsertFallbackLogger()
{
	HandlerDefinition fallbackDef;
	fallbackDef.mSupportedSeverities =
		RF_SEV_ERROR |
		RF_SEV_CRITICAL |
		RF_SEV_MILESTONE |
		RF_SEV_USER_ATTENTION_REQUESTED |
		RF_SEV_UNRECOVERABLE;
	fallbackDef.mUtf8HandlerFunc = FallbackLogger;

	LoggingRouter& router = GetOrCreateGlobalLoggingInstance();
	RF_ASSERT( sFallbackHandlerID == kInvalidHandlerID );
	sFallbackHandlerID = router.RegisterHandler( fallbackDef );
	RF_ASSERT( sFallbackHandlerID != kInvalidHandlerID );
}



void RemoveFallbackLoggerIfPresent()
{
	HandlerID const previousFallbackHandlerID = sFallbackHandlerID.exchange( kInvalidHandlerID, rftl::memory_order::memory_order_acq_rel );
	if( previousFallbackHandlerID != kInvalidHandlerID )
	{
		UnregisterHandler( previousFallbackHandlerID );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
