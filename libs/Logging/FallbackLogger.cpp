#include "stdafx.h"
#include "FallbackLogger.h"

#include "Logging/Logging.h"

#include "core_logging/LoggingRouter.h"

#include "rftl/atomic"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

static rftl::atomic<HandlerID> sFallbackHandlerID = kInvalidHandlerID;

///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent const& event, va_list args )
{
	constexpr size_t kBufSize = 512;

	rftl::array<char, kBufSize> messageBuffer;
	vsnprintf( &messageBuffer[0], kBufSize, event.mTransientMessageFormatString, args );
	*messageBuffer.rbegin() = '\0';

	rftl::array<char, kBufSize> outputBuffer;
	int const bytesParsed = snprintf( &outputBuffer[0], kBufSize, "FALLBACK_LOGGER>>[%s] %s", event.mCategoryKey, &messageBuffer[0] );
	*outputBuffer.rbegin() = '\0';

	puts( &outputBuffer[0] );
	if( bytesParsed >= kBufSize )
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
	fallbackDef.mHandlerFunc = FallbackLogger;

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
}}
