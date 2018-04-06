#include "stdafx.h"
#include "FallbackLogger.h"

#include "PlatformUtils_win32/Debugging.h"
#include "Logging/Logging.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent const& event, va_list args )
{
	constexpr size_t kBufSize = 512;

	std::array<char, kBufSize> messageBuffer;
	vsnprintf( &messageBuffer[0], kBufSize, event.mTransientMessageFormatString, args );
	*messageBuffer.rbegin() = '\0';

	std::array<char, kBufSize> outputBuffer;
	int const bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s]%s\n", event.mCategoryKey, &messageBuffer[0] );
	*outputBuffer.rbegin() = '\0';

	platform::debugging::OutputToDebugger( &outputBuffer[0] );
	if( bytesParsed >= kBufSize )
	{
		platform::debugging::OutputToDebugger( "<TRUNCATED MESSAGE!>" );
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
	RegisterHandler( fallbackDef );
}

///////////////////////////////////////////////////////////////////////////////
}}
