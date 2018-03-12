#pragma once
#include "project.h"

#include "core_logging/LoggingRouter.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackLogger( LoggingRouter const& router, LoggingRouter::LogEvent const& event, va_list args );

void InsertFallbackLogger();

///////////////////////////////////////////////////////////////////////////////
}}
