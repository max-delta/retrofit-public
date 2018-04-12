#pragma once
#include "project.h"

#include "core_logging/LoggingHandler.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LogEvent const& event, va_list args );

void InsertFallbackLogger();
void RemoveFallbackLoggerIfPresent();

///////////////////////////////////////////////////////////////////////////////
}}
