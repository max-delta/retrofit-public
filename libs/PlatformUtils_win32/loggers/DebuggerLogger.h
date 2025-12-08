#pragma once
#include "project.h"

#include "core_logging/LoggingFwd.h"

#include "rftl/format"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API void DebuggerLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args const& args );

///////////////////////////////////////////////////////////////////////////////
}
