#include "stdafx.h"
#include "DebuggerLogger.h"

#include "PlatformUtils_win32/Debugging.h"
#include "Logging/Logging.h"

#include "rftl/limits"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

void DebuggerLogger( logging::LoggingRouter const& router, logging::LogEvent<char> const& event, va_list args )
{
	using namespace RF::logging;

	constexpr size_t kBufSize = 512;

	rftl::array<char, kBufSize> messageBuffer;
	vsnprintf( &messageBuffer[0], kBufSize, event.mTransientMessageFormatString, args );
	*messageBuffer.rbegin() = '\0';

	char const* severity;
	if( event.mSeverityMask & RF_SEV_MILESTONE )
	{
		severity = "MILESTONE";
	}
	else if( event.mSeverityMask & RF_SEV_CRITICAL )
	{
		severity = "CRITICAL";
	}
	else if( event.mSeverityMask & RF_SEV_ERROR )
	{
		severity = "ERROR";
	}
	else if( event.mSeverityMask & RF_SEV_WARNING )
	{
		severity = "WARNING";
	}
	else if( event.mSeverityMask & RF_SEV_INFO )
	{
		severity = "INFO";
	}
	else if( event.mSeverityMask & RF_SEV_DEBUG )
	{
		severity = "DEBUG";
	}
	else if( event.mSeverityMask & RF_SEV_TRACE )
	{
		severity = "TRACE";
	}
	else
	{
		severity = "UNKNOWN";
	}

	rftl::array<char, kBufSize> outputBuffer;
	int bytesParsed;
	if( event.mTransientContextString == nullptr )
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  %s", severity, event.mCategoryKey, &messageBuffer[0] );
	}
	else
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  <%s> %s", severity, event.mCategoryKey, event.mTransientContextString, &messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	platform::debugging::OutputToDebugger( &outputBuffer[0] );
	static_assert( kBufSize <= rftl::numeric_limits<int>::max(), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		platform::debugging::OutputToDebugger( "<TRUNCATED MESSAGE!>" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
