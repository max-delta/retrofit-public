#pragma once
#include "project.h"

#include "core_logging/LoggingFwd.h"

#include "rftl/extension/byte_span.h"
#include "rftl/format"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

struct LogBufferResult
{
	// Exactly as provided, or empty if error occurred
	rftl::byte_span mFullBuffer = {};

	// NOTE: Includes the null terminator
	rftl::byte_span mNullTerminatedOutput = {};
};

struct LogBufferOptions
{
	bool mUseAnsiiEscapeSequences : 1 = false;
};

// Write to buffer, using a standard format
// NOTE: Appends newline at end of output
// NOTE: Will always ensure null termination, even in error case (unless buffer
//  is of size zero, but don't do that...)
LOGGING_API LogBufferResult LogToBuffer(
	rftl::byte_span destination,
	LogBufferOptions options,
	logging::LogEvent<char8_t> const& event,
	rftl::format_args const& args );

///////////////////////////////////////////////////////////////////////////////
}
