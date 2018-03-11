#pragma once
#include "project.h"

#include "core/macros.h"
#include "Logging/Constants.h"

#include <cstdarg>
#include <stddef.h>
#include <array>


// Forwards
namespace RF { namespace logging {
	class LoggingRouter;
}}

namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

#define ___RFLOG_IMPL(CONTEXT, CATEGORY, SEVERITY, ...) \
	do \
	{ \
		constexpr uint64_t ___kCategorySeverityWhitelist = RF_CONCAT(CATEGORY, _SEV_WHITELIST); \
		constexpr uint64_t ___kRestrictedWhitelist = ___kCategorySeverityWhitelist & ___RFLOG_GLOBAL_SEV_WHITELIST_MASK; \
		constexpr uint64_t ___kSeverity = (SEVERITY) & ___kRestrictedWhitelist; \
		if( ___kSeverity != 0 ) \
		{ \
			::RF::logging::details::Log( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ ); \
		} \
	} while( false )

#define RFLOG_TRACE(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_TRACE, __VA_ARGS__ )
#define RFLOG_DEBUG(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_DEBUG, __VA_ARGS__ )
#define RFLOG_INFO(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_INFO, __VA_ARGS__ )
#define RFLOG_WARNING(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_WARNING, __VA_ARGS__ )
#define RFLOG_ERROR(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE, __VA_ARGS__ )
#define RFLOG_CRITICAL(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_CRITICAL, __VA_ARGS__ )
#define RFLOG_FATAL(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE | ::RF::logging::RF_SEV_UNRECOVERABLE, __VA_ARGS__ )

///////////////////////////////////////////////////////////////////////////////

// When specializing contexts, there is a limit to how large you can make the
//  string representation
static constexpr size_t kMaxContextLen = 512;
using LogContextBuffer = std::array<char, kMaxContextLen>;

// Specialize this to add support for your context
// EXAMPLE:
//  template<> void WriteContextString(
//   MyClass const& context,
//   LogContextBuffer buffer );
template<typename Context>
void WriteContextString( Context const& context, LogContextBuffer buffer );

//////////////////////////////////////////////////////////////////////////////

LOGGING_API LoggingRouter& GetOrCreateGlobalLoggingInstance();

//////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename Context>
void Log( Context context, char const* categoryKey, uint64_t severityMask, char const* format, ... );
LOGGING_API void Log( nullptr_t /*context*/, char const* categoryKey, uint64_t severityMask, char const* format, ... );
LOGGING_API void LogVA( char const* context, char const* categoryKey, uint64_t severityMask, char const* format, va_list args );

}
///////////////////////////////////////////////////////////////////////////////
}}

#include "Logging.inl"
