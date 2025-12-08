#include "stdafx.h"
#include "DebuggerLogger.h"

#include "PlatformUtils_win32/Debugging.h"

#include "Logging/StandardLoggingHelpers.h"

#include "rftl/array"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

void DebuggerLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	using namespace RF::logging;

	constexpr size_t kBufSize = 512;
	rftl::array<char8_t, kBufSize> buffer;

	LogBufferResult const result = LogToBuffer(
		rftl::byte_span( buffer.begin(), buffer.end() ),
		LogBufferOptions{},
		event,
		args );
	if( result.mNullTerminatedOutput.empty() )
	{
		return;
	}

	// HACK: Treat UTF-8 as ASCII and hope for the best
	platform::debugging::OutputToDebugger( &result.mNullTerminatedOutput.front<char>() );
}

///////////////////////////////////////////////////////////////////////////////
}
