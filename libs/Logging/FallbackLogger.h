#pragma once
#include "project.h"

#include "core_logging/LoggingFwd.h"

#include "rftl/format"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent<char> const& event, rftl::format_args const& args );

void InsertFallbackLogger();
void RemoveFallbackLoggerIfPresent();

///////////////////////////////////////////////////////////////////////////////
}
