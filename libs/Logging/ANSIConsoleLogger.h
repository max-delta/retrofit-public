#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

LOGGING_API void ANSIConsoleLogger( LoggingRouter const& router, LogEvent const& event, va_list args );

///////////////////////////////////////////////////////////////////////////////
}}
