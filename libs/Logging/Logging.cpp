#include "stdafx.h"
#include "Logging.h"

#include "Logging/FallbackLogger.h"

#include "core/meta/LazyInitSingleton.h"
#include "core_logging/LoggingRouter.h"
#include "core_unicode/BufferConvert.h"
#include "core_math/math_clamps.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void FallbackConversion( Utf16LogContextBuffer& dest, Utf8LogContextBuffer const& source )
{
	unicode::ConvertToUtf16( dest.data(), dest.size(), source.data(), source.size() );
}



void FallbackConversion( Utf32LogContextBuffer& dest, Utf8LogContextBuffer const& source )
{
	unicode::ConvertToUtf32( dest.data(), dest.size(), source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void WriteContextString( char const* const& context, Utf8LogContextBuffer& buffer )
{
	if( context == nullptr )
	{
		buffer.at( 0 ) = '\0';
		return;
	}

	for( size_t i = 0; i < buffer.size(); i++ )
	{
		buffer.at( i ) = context[i];
		if( context[i] == '\0' )
		{
			break;
		}
	}
}



template<>
void WriteContextString( char16_t const* const& context, Utf16LogContextBuffer& buffer )
{
	if( context == nullptr )
	{
		buffer.at( 0 ) = u'\0';
		return;
	}

	for( size_t i = 0; i < buffer.size(); i++ )
	{
		buffer.at( i ) = context[i];
		if( context[i] == u'\0' )
		{
			break;
		}
	}
}



template<>
void WriteContextString( char32_t const* const& context, Utf32LogContextBuffer& buffer )
{
	if( context == nullptr )
	{
		buffer.at( 0 ) = U'\0';
		return;
	}

	for( size_t i = 0; i < buffer.size(); i++ )
	{
		buffer.at( i ) = context[i];
		if( context[i] == U'\0' )
		{
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

LoggingRouter& GetOrCreateGlobalLoggingInstance()
{
	return GetOrInitFunctionStaticScopedSingleton<LoggingRouter>();
}



HandlerID RegisterHandler( HandlerDefinition const& handlerDefinition )
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
	char const* format, ... )
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
	char16_t const* format, ... )
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
	char32_t const* format, ... )
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
}
