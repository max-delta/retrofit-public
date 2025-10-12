#include "stdafx.h"
#include "DebuggerLogger.h"

#include "PlatformUtils_win32/Debugging.h"
#include "Logging/Logging.h"

#include "core_unicode/StringConvert.h"

#include "rftl/extension/variadic_print.h"
#include "rftl/limits"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

void DebuggerLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	using namespace RF::logging;

	// C++20 APIs won't take Unicode, hope that ASCII is good enough
	rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

	constexpr size_t kBufSize = 512;
	rftl::array<char, kBufSize> messageBuffer;
	rftl::var_vformat_to( messageBuffer, legacyFormatString, args );
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
	if( event.mTransientContextString.empty() )
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  %s\n", severity, event.mCategoryKey, &messageBuffer[0] );
	}
	else
	{
		rftl::string const asciiContext = unicode::ConvertToASCII( event.mTransientContextString );
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  <%s> %s\n", severity, event.mCategoryKey, asciiContext.c_str(), &messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	platform::debugging::OutputToDebugger( &outputBuffer[0] );
	static_assert( kBufSize <= static_cast<size_t>( rftl::numeric_limits<int>::max() ), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		platform::debugging::OutputToDebugger( "<TRUNCATED MESSAGE!>\n" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
