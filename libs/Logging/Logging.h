#pragma once
#include "project.h"

#include "Logging/Constants.h"

#include "core_logging/LoggingHandler.h"

#include "core/meta/FailTemplate.h"
#include "core/macros.h"

#include "rftl/array"
#include "rftl/string"
#include "rftl/string_view"
#include "rftl/cstdarg"
#include "rftl/cstddef"
#include "rftl/cstdlib"


// This is not required, as the linker can resolve them without a declaration,
//  but if there's no definition, and the context attempt is in error, this
//  will raise it to a compiler error, which may be helpful when the log site
//  is incorrect but difficult to find
#define RF_STATIC_ASSERT_ON_UNDECLARED_CONTEXT_SPECIALIZATIONS

// Rather than require all the string literals to have a 'u8' prefix, allow
//  normal string literals to be reinterpreted as UTF-8, then though it's a bit
//  dubious, since the practicality outweighs the correctness
#define RF_TREAT_LOG_ASCII_FORMAT_STRINGS_AS_UTF8
#define RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8

// Forwards
namespace RF::logging {
class LoggingRouter;
}

namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////
namespace details {

#define ___RFLOG_WHITELIST_IMPL( CATEGORY, SEVERITY ) \
	constexpr uint64_t ___kCategorySeverityWhitelist = RF_CONCAT( CATEGORY, _SEV_WHITELIST ); \
	constexpr uint64_t ___kRestrictedWhitelist = ___kCategorySeverityWhitelist & ___RFLOG_GLOBAL_SEV_WHITELIST_MASK; \
	constexpr uint64_t ___kSeverity = ( SEVERITY ) & ___kRestrictedWhitelist;

#define ___RFLOG_IMPL( CONTEXT, CATEGORY, SEVERITY, ... ) \
	do \
	{ \
		___RFLOG_WHITELIST_IMPL( CATEGORY, SEVERITY ); \
		if( ___kSeverity != 0 ) \
		{ \
			constexpr uint64_t ___kBannedBits = ___kSeverity & ___RFLOG_GLOBAL_SEV_BLACKLIST; \
			if( ___kBannedBits == 0 ) \
			{ \
				::RF::logging::details::Log( CONTEXT, CATEGORY, SEVERITY, RF_FILENAME(), static_cast<size_t>( __LINE__ ), __VA_ARGS__ ); \
			} \
		} \
	} while( false )

#define ___RFLOG_ABORT_IMPL( CONTEXT, CATEGORY, SEVERITY, ... ) \
	do \
	{ \
		___RFLOG_IMPL( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ ); \
		rftl::abort(); \
	} while( false )

#define ___RFLOG_TEST_IMPL( TEST, ACTION ) \
	do \
	{ \
		if( !!!( TEST ) ) \
		{ \
			ACTION; \
		} \
	} while( false )

}
///////////////////////////////////////////////////////////////////////////////

// For extremely verbose logging that could significantly impact performance or
//  generate extraordinary amounts of data unless under controlled inspection
// Examples: Token is read from file, node is traversed
// Expected user action: Examine alongside debugger
#define RFLOG_TRACE( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_TRACE, __VA_ARGS__ )

// For verbose logging that is too noisy to be of value to anyone besides the
//  owner of the code
// Examples: Texture information after loading, duration of a long operation
// Expected user action: Examine against expectations
#define RFLOG_DEBUG( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_DEBUG, __VA_ARGS__ )

// For general informational messages that are useful to a wide audience
// Examples: File is loaded, entity is created/destroyed
// Expected user action: Ignore, occasionally cross-reference against errors
#define RFLOG_INFO( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_INFO, __VA_ARGS__ )

// For a failure or quirk that is undesirable, but benign and safely handled
// Examples: Texture is unreasonably large, expensive action has no effect
// Expected user action: Cleanup when practical
#define RFLOG_WARNING( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_WARNING, __VA_ARGS__ )

// For a failure that is unreliably handled, or diverts significantly from
//  expected program flow, likely with detrimental observable side-effects
// Examples: Can't load asset, character's head is missing
// Expected user action: Fix when practical
#define RFLOG_ERROR( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE, __VA_ARGS__ )

// For an extraordinary class of failures that may constitute emergencies in a
//  production environment or retail release, that could have business impact,
//  even if the code can continue normally
// NOTE: Assume this is enabled in retail builds
// Examples: Significant datacenter outage, improper monetary transaction
// Expected user action: Pagers go off, panic
#define RFLOG_CRITICAL( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_CRITICAL, __VA_ARGS__ )

// For a catastrophic and unrecoverable error, where continuing the process
//  would have terrible indirect effects later that would be difficult to
//  diagnose if execution was allowed to continue
// NOTE: Assume this is enabled in retail builds
// Examples: Memory corruption detected, critical system failed to initialize
// Expected user action: Investigate and fix
#define RFLOG_FATAL( CONTEXT, CATEGORY, ... ) ___RFLOG_ABORT_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE | ::RF::logging::RF_SEV_UNRECOVERABLE, __VA_ARGS__ )
#define RFLOG_TEST_AND_FATAL( TEST, CONTEXT, CATEGORY, ... ) ___RFLOG_TEST_IMPL( TEST, ___RFLOG_ABORT_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_CALLSTACK_VALUABLE | ::RF::logging::RF_SEV_UNRECOVERABLE, __VA_ARGS__ ) )

// For a major event that signifies a meaningful landmark in the application
// Examples: Player dies, level transition, cutscene, quit to main menu
// Expected user action: Ignore, occasionally use as a guide when viewing logs
#define RFLOG_MILESTONE( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_MILESTONE, __VA_ARGS__ )

// For errors that a user should be made immediately aware of
// Examples: Can't load asset, asset is invalid
// Expected user action: Investigate and fix
#define RFLOG_NOTIFY( CONTEXT, CATEGORY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_USER_ATTENTION_REQUESTED, __VA_ARGS__ )
#define RFLOG_TEST_AND_NOTIFY( TEST, CONTEXT, CATEGORY, ... ) ___RFLOG_TEST_IMPL( TEST, ___RFLOG_IMPL( CONTEXT, CATEGORY, ::RF::logging::RF_SEV_ERROR | ::RF::logging::RF_SEV_USER_ATTENTION_REQUESTED, __VA_ARGS__ ) )

// For custom log severities, not recommented to be used directly, should be
//  wrapped in a macro to centralize authoring of potential log types
#define RFLOG_CUSTOM( CONTEXT, CATEGORY, SEVERITY, ... ) ___RFLOG_IMPL( CONTEXT, CATEGORY, SEVERITY, __VA_ARGS__ )

///////////////////////////////////////////////////////////////////////////////

// HACK: Temporary markup shims for noting format args that will need attention
//  when switching to C++20 format style logging
// NOTE: These were found by more complex temp macro changes that would fail on
//  any type that passed in an arg past the format string
#define RFLOGF_TRACE RFLOG_TRACE
#define RFLOGF_DEBUG RFLOG_DEBUG
#define RFLOGF_INFO RFLOG_INFO
#define RFLOGF_WARNING RFLOG_WARNING
#define RFLOGF_ERROR RFLOG_ERROR
#define RFLOGF_CRITICAL RFLOG_CRITICAL
#define RFLOGF_FATAL RFLOG_FATAL
#define RFLOGF_TEST_AND_FATAL RFLOG_TEST_AND_FATAL
#define RFLOGF_MILESTONE RFLOG_MILESTONE
#define RFLOGF_NOTIFY RFLOG_NOTIFY
#define RFLOGF_TEST_AND_NOTIFY RFLOG_TEST_AND_NOTIFY
#define RFLOGF_CUSTOM RFLOG_CUSTOM

///////////////////////////////////////////////////////////////////////////////

// When specializing contexts, there is a limit to how large you can make the
//  string representation
static constexpr size_t kMaxContextLen = 512;
using Utf8LogContextBuffer = rftl::array<char8_t, kMaxContextLen / sizeof( char8_t )>;
using Utf16LogContextBuffer = rftl::array<char16_t, kMaxContextLen / sizeof( char16_t )>;
using Utf32LogContextBuffer = rftl::array<char32_t, kMaxContextLen / sizeof( char32_t )>;

// Some fallbacks are provided for context specialization
LOGGING_API void FallbackConversion( Utf16LogContextBuffer& dest, Utf8LogContextBuffer const& source );
LOGGING_API void FallbackConversion( Utf32LogContextBuffer& dest, Utf8LogContextBuffer const& source );

// Specialize these to add support for your context
// EXAMPLE:
//  template<> void WriteContextString(
//   MyClass const& context,
//   Utf8LogContextBuffer& buffer );
// NOTE: Will be called from multiple threads
template<typename Context>
void WriteContextString( Context const& context, Utf8LogContextBuffer& buffer );
template<typename Context>
void WriteContextString( Context const& context, Utf16LogContextBuffer& buffer )
{
	// In the absence of a UTF-16 specialization, UTF-8 will be used
	Utf8LogContextBuffer temp = {};
	WriteContextString( context, temp );
	FallbackConversion( buffer, temp );
}
template<typename Context>
void WriteContextString( Context const& context, Utf32LogContextBuffer& buffer )
{
	// In the absence of a UTF-32 specialization, UTF-8 will be used
	Utf8LogContextBuffer temp = {};
	WriteContextString( context, temp );
	FallbackConversion( buffer, temp );
}

#ifdef RF_STATIC_ASSERT_ON_UNDECLARED_CONTEXT_SPECIALIZATIONS
template<typename Context>
void WriteContextString( Context const& context, Utf8LogContextBuffer& buffer )
{
	static_assert( FailTemplate<Context>(),
		"No declaration was found for this context. The linker may still be"
		" able to locate and resolve it, but this is currently flagged to"
		" cause a compilation error instead. This usually means you're trying"
		" to use a variable as a context in logging, but that variable type"
		" doesn't have a specialization (which will error on link), or that"
		" specialization isn't visible from the current compilation unit"
		" (it would successfully link later if not blocked by this assert)" );
}
#endif

//////////////////////////////////////////////////////////////////////////////

// Some stock generic contexts
#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void LOGGING_API WriteContextString( char const* const& context, Utf8LogContextBuffer& buffer );
#endif
template<>
void LOGGING_API WriteContextString( char8_t const* const& context, Utf8LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( char16_t const* const& context, Utf16LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( char32_t const* const& context, Utf32LogContextBuffer& buffer );
#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void LOGGING_API WriteContextString( rftl::basic_string_view<char> const& context, Utf8LogContextBuffer& buffer );
#endif
template<>
void LOGGING_API WriteContextString( rftl::basic_string_view<char8_t> const& context, Utf8LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( rftl::basic_string_view<char16_t> const& context, Utf8LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( rftl::basic_string_view<char32_t> const& context, Utf8LogContextBuffer& buffer );
#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void LOGGING_API WriteContextString( rftl::basic_string<char> const& context, Utf8LogContextBuffer& buffer );
#endif
template<>
void LOGGING_API WriteContextString( rftl::basic_string<char8_t> const& context, Utf8LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( rftl::basic_string<char16_t> const& context, Utf8LogContextBuffer& buffer );
template<>
void LOGGING_API WriteContextString( rftl::basic_string<char32_t> const& context, Utf8LogContextBuffer& buffer );

//////////////////////////////////////////////////////////////////////////////

LOGGING_API LoggingRouter& GetOrCreateGlobalLoggingInstance();
LOGGING_API HandlerID RegisterHandler( HandlerDefinition const& handlerDefinition );
LOGGING_API bool UnregisterHandler( HandlerID handlerID );

//////////////////////////////////////////////////////////////////////////////
namespace details {

#ifdef RF_TREAT_LOG_ASCII_FORMAT_STRINGS_AS_UTF8
template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::string_view format,
	ArgsT&&... args );
#endif
template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u8string_view format,
	ArgsT&&... args );
template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u16string_view format,
	ArgsT&&... args );
template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u32string_view format,
	ArgsT&&... args );
#ifdef RF_TREAT_LOG_ASCII_FORMAT_STRINGS_AS_UTF8
template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::string_view format, 
	ArgsT&&... args );
#endif
template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u8string_view format, 
	ArgsT&&... args );
template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u16string_view format, 
	ArgsT&&... args );
template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u32string_view format, 
	ArgsT&&... args );
LOGGING_API void LogVA(
	rftl::u8string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u8string_view format,
	rftl::format_args&& args );
LOGGING_API void LogVA(
	rftl::u16string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u16string_view format,
	rftl::format_args&& args );
LOGGING_API void LogVA(
	rftl::u32string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	rftl::u32string_view format,
	rftl::format_args&& args );

}
///////////////////////////////////////////////////////////////////////////////
}

#include "Logging.inl"
