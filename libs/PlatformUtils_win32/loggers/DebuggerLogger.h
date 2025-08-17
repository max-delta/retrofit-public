#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API void DebuggerLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args&& args );

///////////////////////////////////////////////////////////////////////////////
}
