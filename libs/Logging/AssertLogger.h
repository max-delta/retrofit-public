#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

LOGGING_API void AssertLogger( LoggingRouter const& router, LogEvent<char8_t> const& event, rftl::format_args const& args );

///////////////////////////////////////////////////////////////////////////////
}
