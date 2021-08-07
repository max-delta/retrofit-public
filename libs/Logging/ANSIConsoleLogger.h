#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

LOGGING_API void ANSIConsoleLogger( LoggingRouter const& router, LogEvent<char> const& event, va_list args );

///////////////////////////////////////////////////////////////////////////////
}
