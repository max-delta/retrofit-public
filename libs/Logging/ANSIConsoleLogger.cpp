#include "stdafx.h"
#include "ANSIConsoleLogger.h"

#include "PlatformUtils_win32/Console.h"
#include "Logging/Logging.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

void ANSIConsoleLogger( LoggingRouter const& router, LogEvent const& event, va_list args )
{
	constexpr size_t kBufSize = 512;

	rftl::array<char, kBufSize> messageBuffer;
	vsnprintf( &messageBuffer[0], kBufSize, event.mTransientMessageFormatString, args );
	*messageBuffer.rbegin() = '\0';

	char const* severity;
	if( event.mSeverityMask & RF_SEV_MILESTONE )
	{
		severity = "\x1b[1;36mMILESTONE";
	}
	else if( event.mSeverityMask & RF_SEV_CRITICAL )
	{
		severity = "\x1b[1;31mCRITICAL";
	}
	else if( event.mSeverityMask & RF_SEV_ERROR )
	{
		severity = "\x1b[1;31mERROR";
	}
	else if( event.mSeverityMask & RF_SEV_WARNING )
	{
		severity = "\x1b[1;33mWARNING";
	}
	else if( event.mSeverityMask & RF_SEV_INFO )
	{
		severity = "\x1b[0mINFO";
	}
	else if( event.mSeverityMask & RF_SEV_DEBUG )
	{
		severity = "\x1b[1;30mDEBUG";
	}
	else if( event.mSeverityMask & RF_SEV_TRACE )
	{
		severity = "\x1b[1;30mTRACE";
	}
	else
	{
		severity = "\x1b[0mUNKNOWN";
	}

	rftl::array<char, kBufSize> outputBuffer;
	int bytesParsed;
	if( event.mTransientContextString == nullptr )
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s\x1b[0m][%s]  %s", severity, event.mCategoryKey, &messageBuffer[0] );
	}
	else
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s\x1b[0m][%s]  <%s> %s", severity, event.mCategoryKey, event.mTransientContextString, &messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	puts( &outputBuffer[0] );
	static_assert( kBufSize <= std::numeric_limits<int>::max(), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		puts( "<TRUNCATED MESSAGE!>" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
