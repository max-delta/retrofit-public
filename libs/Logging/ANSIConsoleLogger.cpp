#include "stdafx.h"
#include "ANSIConsoleLogger.h"

#include "PlatformUtils_win32/Console.h"
#include "Logging/Logging.h"

#include "core_unicode/StringConvert.h"

#include "rftl/extension/bounded_overwrite_iterator.h"
#include "rftl/limits"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void ANSIConsoleLogger( LoggingRouter const& router, LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	// C++20 APIs won't take Unicode, hope that ASCII is good enough
	rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

	constexpr size_t kBufSize = 512;
	rftl::array<char, kBufSize> messageBuffer;
	{
		rftl::bounded_forward_overwrite_iterator out( messageBuffer );
		out = rftl::vformat_to( out, legacyFormatString, args );
		*out = '\0';
		*messageBuffer.rbegin() = '\0';
	}

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
	if( event.mTransientContextString.empty() )
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s\x1b[0m][%s]  %s", severity, event.mCategoryKey, &messageBuffer[0] );
	}
	else
	{
		rftl::string const asciiContext = unicode::ConvertToASCII( event.mTransientContextString );
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s\x1b[0m][%s]  <%s> %s", severity, event.mCategoryKey, asciiContext.c_str(), &messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	// NOTE: At time of writing, there's a bug in the default Windows console
	//  that garbles any newline that hits the final column if ANSI terminal
	//  output is enabled. This is NOT threading related or buffer related,
	//  this puts(...) call is correctly implemented.
	// SEE: https://stackoverflow.com/questions/66244924/windows-console-conhost-discards-newline-when-output-matches-witdth-of-the-win
	puts( &outputBuffer[0] );
	static_assert( kBufSize <= static_cast<size_t>( rftl::numeric_limits<int>::max() ), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		puts( "<TRUNCATED MESSAGE!>" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
