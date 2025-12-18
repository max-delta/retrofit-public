#include "stdafx.h"

#include "StandardLoggingHelpers.h"

#include "Logging/Constants.h"

#include "core_logging/LogEvent.h"
#include "core_math/math_casts.h"
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

	char const* severity;
	if( event.mSeverityMask & RF_SEV_MILESTONE )
	{
		severity = doAnsi ? "\x1b[1;36mMILESTONE" : "MILESTONE";
	}
	else if( event.mSeverityMask & RF_SEV_CRITICAL )
	{
		severity = doAnsi ? "\x1b[1;31mCRITICAL" : "CRITICAL";
	}
	else if( event.mSeverityMask & RF_SEV_ERROR )
	{
		severity = doAnsi ? "\x1b[1;31mERROR" : "ERROR";
	}
	else if( event.mSeverityMask & RF_SEV_WARNING )
	{
		severity = doAnsi ? "\x1b[1;33mWARNING" : "WARNING";
	}
	else if( event.mSeverityMask & RF_SEV_INFO )
	{
		severity = doAnsi ? "\x1b[0mINFO" : "INFO";
	}
	else if( event.mSeverityMask & RF_SEV_DEBUG )
	{
		severity = doAnsi ? "\x1b[1;30mDEBUG" : "DEBUG";
	}
	else if( event.mSeverityMask & RF_SEV_TRACE )
	{
		severity = doAnsi ? "\x1b[1;30mTRACE" : "TRACE";
	}
	else
	{
		severity = doAnsi ? "\x1b[0mUNKNOWN" : "UNKNOWN";
	}

	rftl::span<char> const outputBuffer = destination.to_typed_span<char>();
	int bytesParsed;
	if( event.mTransientContextString.empty() )
	{
		if( doAnsi )
		{
			bytesParsed = rftl::snprintf( outputBuffer.data(), outputBuffer.size(), "[%s\x1b[0m][%s]  %s", severity, event.mCategoryKey, &messageBuffer[0] );
		}
		else
		{
			bytesParsed = rftl::snprintf( outputBuffer.data(), outputBuffer.size(), "[%s][%s]  %s\n", severity, event.mCategoryKey, &messageBuffer[0] );
		}
	}
	else
	{
		rftl::string const asciiContext = unicode::ConvertToASCII( event.mTransientContextString );
		if( doAnsi )
		{
			bytesParsed = rftl::snprintf( outputBuffer.data(), outputBuffer.size(), "[%s\x1b[0m][%s]  <%s> %s", severity, event.mCategoryKey, asciiContext.c_str(), &messageBuffer[0] );
		}
		else
		{
			bytesParsed = rftl::snprintf( outputBuffer.data(), outputBuffer.size(), "[%s][%s]  <%s> %s\n", severity, event.mCategoryKey, asciiContext.c_str(), &messageBuffer[0] );
		}
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
