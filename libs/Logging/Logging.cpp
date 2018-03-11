#include "stdafx.h"
#include "Logging.h"

#include "core/meta/LazyInitSingleton.h"
#include "core_logging/LoggingRouter.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

LoggingRouter& GetOrCreateGlobalLoggingInstance()
{
	return GetOrInitFunctionStaticScopedSingleton<LoggingRouter>();
}

///////////////////////////////////////////////////////////////////////////////
namespace details {

void Log( nullptr_t, char const * categoryKey, uint64_t severityMask, char const * format, ... )
{
	va_list args;
	va_start( args, format );
	details::LogVA( nullptr, categoryKey, severityMask, format, args );
	va_end( args );
}



void LogVA( char const* context, char const* categoryKey, uint64_t severityMask, char const* format, va_list args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, format, args );
}

}
///////////////////////////////////////////////////////////////////////////////
}}
