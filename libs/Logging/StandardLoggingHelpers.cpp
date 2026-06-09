#include "stdafx.h"

#include "StandardLoggingHelpers.h"

#include "Logging/Constants.h"

#include "core_logging/LogEvent.h"
#include "core_math/math_casts.h"
#include "core_terminal/CommonAnsiEscapes.h"
#include "core_unicode/StringConvert.h"

#include "rftl/array"
#include "rftl/extension/string_copy.h"
#include "rftl/extension/variadic_print.h"
#include "rftl/string_view"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

LogBufferResult LogToBuffer( rftl::byte_span destination, LogBufferOptions options, logging::LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	static constexpr rftl::u8string_view kTruncated = u8"<TRUNCATED MESSAGE!>\n";
	static constexpr rftl::span kTruncatedWithNull( kTruncated.begin(), kTruncated.size() );
	static constexpr size_t kMinBufferSizeForTruncation = kTruncatedWithNull.size();

	if( destination.size() < kMinBufferSizeForTruncation )
	{
		RF_DBGFAIL();
		return {};
	}

	LogBufferResult retVal = {};
	retVal.mFullBuffer = destination;

	// C++20 APIs won't take Unicode, hope that ASCII is good enough
	rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

	constexpr size_t kMessageBufSize = 512;
	rftl::array<char, kMessageBufSize> messageBuffer;
	rftl::var_vformat_to( messageBuffer, legacyFormatString, args );
	*messageBuffer.rbegin() = '\0';

	bool const doAnsi = options.mUseAnsiiEscapeSequences;

	char const* preSeverity;
	char const* severity;
	char const* const postSeverity = doAnsi ? term::ansi::csi::sgr::kReset : "";
	if( event.mSeverityMask & RF_SEV_MILESTONE )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kBrightCyan : "";
		severity = "MILESTONE";
	}
	else if( event.mSeverityMask & RF_SEV_CRITICAL )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kRed : "";
		severity = "CRITICAL";
	}
	else if( event.mSeverityMask & RF_SEV_ERROR )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kBrightRed : "";
		severity = "ERROR";
	}
	else if( event.mSeverityMask & RF_SEV_WARNING )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kBrightYellow : "";
		severity = "WARNING";
	}
	else if( event.mSeverityMask & RF_SEV_INFO )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kReset : "";
		severity = "INFO";
	}
	else if( event.mSeverityMask & RF_SEV_DEBUG )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kBrightBlack : "";
		severity = "DEBUG";
	}
	else if( event.mSeverityMask & RF_SEV_TRACE )
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kBrightBlack : "";
		severity = "TRACE";
	}
	else
	{
		preSeverity = doAnsi ? term::ansi::csi::sgr::kReset : "";
		severity = "UNKNOWN";
	}

	rftl::span<char> const outputBuffer = destination.to_typed_span<char>();
	int bytesParsed;
	if( event.mTransientContextString.empty() )
	{
		bytesParsed = rftl::snprintf(
			outputBuffer.data(),
			outputBuffer.size(),
			"[%s%s%s][%s]  %s",
			preSeverity,
			severity,
			postSeverity,
			event.mCategoryKey,
			&messageBuffer[0] );
	}
	else
	{
		rftl::string const asciiContext = unicode::ConvertToASCII( event.mTransientContextString );
		bytesParsed = rftl::snprintf(
			outputBuffer.data(),
			outputBuffer.size(),
			"[%s%s%s][%s]  <%s> %s",
			preSeverity,
			severity,
			postSeverity,
			event.mCategoryKey,
			asciiContext.c_str(),
			&messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	RF_ASSERT_MSG( outputBuffer.size() <= static_cast<size_t>( rftl::numeric_limits<int>::max() ), "Int is a terrible type, and can't handle this buffer size" );
	if( bytesParsed >= static_cast<int>( outputBuffer.size() ) )
	{
		RF_ASSERT( outputBuffer.size() - kMinBufferSizeForTruncation < outputBuffer.size() );
		rftl::span<char> const reservedForTruncation = outputBuffer.subspan( outputBuffer.size() - kMinBufferSizeForTruncation );
		rftl::string_view const HACK_asAscii( reinterpret_cast<char const*>( kTruncatedWithNull.data() ), kTruncatedWithNull.size() );
		rftl::string_copy( reservedForTruncation, HACK_asAscii );
		*outputBuffer.rbegin() = '\0';

		// Whole buffer was used, had to truncate as well
		retVal.mNullTerminatedOutput = destination;
		return retVal;
	}

	// Buffer wasn't fully used
	retVal.mNullTerminatedOutput = destination.subspan( 0, math::integer_cast<size_t>( bytesParsed ) + sizeof( '\0' ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
