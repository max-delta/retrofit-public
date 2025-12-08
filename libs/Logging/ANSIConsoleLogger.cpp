#include "stdafx.h"
#include "ANSIConsoleLogger.h"

#include "Logging/StandardLoggingHelpers.h"

#include "rftl/array"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void ANSIConsoleLogger( LoggingRouter const& router, LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	constexpr size_t kBufSize = 512;
	rftl::array<char8_t, kBufSize> buffer;

	LogBufferResult const result = LogToBuffer(
		rftl::byte_span( buffer.begin(), buffer.end() ),
		LogBufferOptions{ .mUseAnsiiEscapeSequences = true },
		event,
		args );
	if( result.mNullTerminatedOutput.empty() )
	{
		return;
	}

	// NOTE: At time of writing, there's a bug in the default Windows console
	//  that garbles any newline that hits the final column if ANSI terminal
	//  output is enabled. This is NOT threading related or buffer related,
	//  this puts(...) call is correctly implemented.
	// SEE: https://stackoverflow.com/questions/66244924/windows-console-conhost-discards-newline-when-output-matches-witdth-of-the-win
	// HACK: Treat UTF-8 as ASCII and hope for the best
	puts( &result.mNullTerminatedOutput.front<char>() );
}

///////////////////////////////////////////////////////////////////////////////
}
