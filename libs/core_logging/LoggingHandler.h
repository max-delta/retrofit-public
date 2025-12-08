#pragma once
#include "core_logging/LogEvent.h"

#include "rftl/format"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

using UTF8HandlerFunc = void ( * )( LoggingRouter const&, LogEvent<char8_t> const&, rftl::format_args const& args );
using UTF16HandlerFunc = void ( * )( LoggingRouter const&, LogEvent<char16_t> const&, rftl::format_args const& args );
using UTF32HandlerFunc = void ( * )( LoggingRouter const&, LogEvent<char32_t> const&, rftl::format_args const& args );

struct HandlerDefinition
{
	// May recieve additional severities on multi-severity events
	SeverityMask mSupportedSeverities = 0;

	// Must support multi-threading
	// NOTE: Atleast one handler must be specified, up/down conversion will be
	//  done as needed when some handlers are null
	UTF8HandlerFunc mUtf8HandlerFunc = nullptr;
	UTF16HandlerFunc mUtf16HandlerFunc = nullptr;
	UTF32HandlerFunc mUtf32HandlerFunc = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
