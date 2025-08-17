#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent<char> const& event, rftl::format_args&& args );

void InsertFallbackLogger();
void RemoveFallbackLoggerIfPresent();

///////////////////////////////////////////////////////////////////////////////
}
