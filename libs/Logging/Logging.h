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
namespace details {

#define ___RFLOG_WHITELIST_IMPL(CATEGORY, SEVERITY) \
		constexpr uint64_t ___kCategorySeverityWhitelist = RF_CONCAT(CATEGORY, _SEV_WHITELIST); \
		constexpr uint64_t ___kRestrictedWhitelist = ___kCategorySeverityWhitelist & ___RFLOG_GLOBAL_SEV_WHITELIST_MASK; \
		constexpr uint64_t ___kSeverity = (SEVERITY) & ___kRestrictedWhitelist;

#define ___RFLOG_IMPL(CONTEXT, CATEGORY, SEVERITY, ...) \
	do \
	{ \
		___RFLOG_WHITELIST_IMPL(CATEGORY, SEVERITY); \
		if( ___kSeverity != 0 ) \
		{ \
			::RF::logging::details::Log( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ ); \
		} \
	} while( false )

#define ___RFLOG_ABORT_IMPL(CONTEXT, CATEGORY, SEVERITY, ...) \
	do \
	{ \
		___RFLOG_IMPL( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ ); \
		std::abort(); \
	} while( false )

#define ___RFLOG_TEST_IMPL(TEST, ACTION) \
	do \
	{ \
		if( !!!(TEST) ) \
		{ \
			ACTION; \
		} \
	} while (false)

}
///////////////////////////////////////////////////////////////////////////////

// For extremely verbose logging that could significantly impact performance or
//  generate extraordinary amounts of data unless under controlled inspection
// Examples: Token is read from file, node is traversed
// Expected user action: Examine alongside debugger
#define RFLOG_TRACE(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_TRACE, __VA_ARGS__ )

// For verbose logging that is too noisy to be of value to anyone besides the
//  owner of the code
// Examples: Texture information after loading, duration of a long operation
// Expected user action: Examine against expectations
#define RFLOG_DEBUG(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_DEBUG, __VA_ARGS__ )

// For general informational messages that are useful to a wide audience
// Examples: File is loaded, entity is created/destroyed
// Expected user action: Ignore, occasionally cross-reference against errors
#define RFLOG_INFO(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_INFO, __VA_ARGS__ )

// For a failure or quirk that is undesirable, but benign and safely handled
// Examples: Texture is unreasonably large, expensive action has no effect
// Expected user action: Cleanup when practical
#define RFLOG_WARNING(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_WARNING, __VA_ARGS__ )

// For a failure that is unreliably handled, or diverts significantly from
//  expected program flow, likely with detrimental observable side-effects
// Examples: Can't load asset, character's head is missing
// Expected user action: Fix when practical
#define RFLOG_ERROR(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE, __VA_ARGS__ )

// For an extraordinary class of failures that may constitute emergencies in a
//  production environment or retail release, that could have business impact,
//  even if the code can continue normally
// NOTE: Assume this is enabled in retail builds
// Examples: Significant datacenter outage, improper monetary transaction
// Expected user action: Pagers go off, panic
#define RFLOG_CRITICAL(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_CRITICAL, __VA_ARGS__ )

// For a catastrophic and unrecoverable error, where continuing the process
//  would have terrible indirect effects later that would be difficult to
//  diagnose if execution was allowed to continue
// NOTE: Assume this is enabled in retail builds
// Examples: Memory corruption detected, critical system failed to initialize
// Expected user action: Investigate and fix
#define RFLOG_FATAL(CONTEXT, CATEGORY, ...) ___RFLOG_ABORT_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE | ::RF::logging::RF_SEV_UNRECOVERABLE, __VA_ARGS__ )

// For a major event that signifies a meaningful landmark in the application
// Examples: Player dies, level transition, cutscene, quit to main menu
// Expected user action: Ignore, occasionally use as a guide when viewing logs
#define RFLOG_MILESTONE(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_MILESTONE, __VA_ARGS__ )

// For errors that a user should be made immediately aware of
// Examples: Can't load asset, asset is invalid
// Expected user action: Investigate and fix
#define RFLOG_NOTIFY(CONTEXT, CATEGORY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_USER_ATTENTION_REQUESTED, __VA_ARGS__ )
#define RFLOG_TEST_AND_NOTIFY(TEST, CONTEXT, CATEGORY, ...) ___RFLOG_TEST_IMPL( TEST, ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_USER_ATTENTION_REQUESTED, __VA_ARGS__ ) )

// For custom log severities, not recommented to be used directly, should be
//  wrapped in a macro to centralize authoring of potential log types
#define RFLOG_CUSTOM(CONTEXT, CATEGORY, SEVERITY, ...) ___RFLOG_IMPL( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ )

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
// NOTE: Will be called from multiple threads
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
