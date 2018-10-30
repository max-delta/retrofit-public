#include "stdafx.h"
#include "Logging.h"

#include "Logging/FallbackLogger.h"

#include "core/meta/LazyInitSingleton.h"
#include "core_logging/LoggingRouter.h"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

LoggingRouter& GetOrCreateGlobalLoggingInstance()
{
	return GetOrInitFunctionStaticScopedSingleton<LoggingRouter>();
}



HandlerID RegisterHandler( HandlerDefinition const & handlerDefinition )
{
	RemoveFallbackLoggerIfPresent();

	LoggingRouter& router = GetOrCreateGlobalLoggingInstance();
	return router.RegisterHandler( handlerDefinition );
}



bool UnregisterHandler( HandlerID handlerID )
{
	LoggingRouter& router = GetOrCreateGlobalLoggingInstance();
	return router.UnregisterHandler( handlerID );
}

///////////////////////////////////////////////////////////////////////////////
namespace details {

void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char const * format, ... )
{
	va_list args;
	va_start( args, format );
	details::LogVA( nullptr, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char16_t const * format, ... )
{
	va_list args;
	va_start( args, format );
	details::LogVA( nullptr, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char32_t const * format, ... )
{
	va_list args;
	va_start( args, format );
	details::LogVA( nullptr, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void LogVA(
	char const* context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char const* format,
	va_list args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, args );
}



void LogVA(
	char16_t const* context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char16_t const* format,
	va_list args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, args );
}



void LogVA(
	char32_t const* context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char32_t const* format,
	va_list args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, args );
}

}
///////////////////////////////////////////////////////////////////////////////
}}
