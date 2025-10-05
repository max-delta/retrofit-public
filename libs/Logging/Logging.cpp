#include "stdafx.h"
#include "Logging.h"

#include "Logging/FallbackLogger.h"

#include "core/meta/LazyInitSingleton.h"
#include "core_logging/LoggingRouter.h"
#include "core_unicode/BufferConvert.h"
#include "core_math/math_clamps.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT, typename BufferT>
void WriteContextStringFromPointer( CharT const* const& context, BufferT& buffer, CharT const terminator )
{
	if( context == nullptr )
	{
		buffer.at( 0 ) = terminator;
		return;
	}

	for( size_t i = 0; i < buffer.size(); i++ )
	{
		buffer.at( i ) = context[i];
		if( context[i] == terminator )
		{
			break;
		}
	}
}



template<typename CharT, typename BufferT>
void WriteContextStringFromView( rftl::basic_string_view<CharT> const& context, BufferT& buffer, CharT const terminator )
{
	for( size_t i = 0; i < buffer.size(); i++ )
	{
		if( i >= context.size() )
		{
			buffer.at( i ) = terminator;
			break;
		}

		buffer.at( i ) = context[i];
	}

	*buffer.rbegin() = terminator;
}



template<typename CharT, typename BufferT>
void WriteContextStringFromString( rftl::basic_string<CharT> const& context, BufferT& buffer, CharT const terminator )
{
	rftl::basic_string_view<CharT> const view = context;
	WriteContextStringFromView( view, buffer, terminator );
}

}
///////////////////////////////////////////////////////////////////////////////

void FallbackConversion( Utf16LogContextBuffer& dest, Utf8LogContextBuffer const& source )
{
	unicode::ConvertToUtf16( dest, { source.data(), source.size() } );
}



void FallbackConversion( Utf32LogContextBuffer& dest, Utf8LogContextBuffer const& source )
{
	unicode::ConvertToUtf32( dest, { source.data(), source.size() } );
}

///////////////////////////////////////////////////////////////////////////////

#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void WriteContextString( char const* const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromPointer( reinterpret_cast<char8_t const*>( context ), buffer, u8'\0' );
}
#endif

template<>
void WriteContextString( char8_t const* const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromPointer( context, buffer, u8'\0' );
}

template<>
void WriteContextString( char16_t const* const& context, Utf16LogContextBuffer& buffer )
{
	details::WriteContextStringFromPointer( context, buffer, u'\0' );
}

template<>
void WriteContextString( char32_t const* const& context, Utf32LogContextBuffer& buffer )
{
	details::WriteContextStringFromPointer( context, buffer, U'\0' );
}



#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void WriteContextString( rftl::basic_string_view<char> const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromView( rftl::basic_string_view<char8_t>( reinterpret_cast<char8_t const*>( context.data() ), context.size() ), buffer, u8'\0' );
}
#endif

template<>
void WriteContextString( rftl::basic_string_view<char8_t> const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromView( context, buffer, u8'\0' );
}

template<>
void WriteContextString( rftl::basic_string_view<char16_t> const& context, Utf16LogContextBuffer& buffer )
{
	details::WriteContextStringFromView( context, buffer, u'\0' );
}

template<>
void WriteContextString( rftl::basic_string_view<char32_t> const& context, Utf32LogContextBuffer& buffer )
{
	details::WriteContextStringFromView( context, buffer, U'\0' );
}



#ifdef RF_TREAT_LOG_ASCII_CONTEXT_STRINGS_AS_UTF8
template<>
void WriteContextString( rftl::basic_string<char> const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromView( rftl::basic_string_view<char8_t>( reinterpret_cast<char8_t const*>( context.data() ), context.size() ), buffer, u8'\0' );
}
#endif

template<>
void WriteContextString( rftl::basic_string<char8_t> const& context, Utf8LogContextBuffer& buffer )
{
	details::WriteContextStringFromString( context, buffer, u8'\0' );
}

template<>
void WriteContextString( rftl::basic_string<char16_t> const& context, Utf16LogContextBuffer& buffer )
{
	details::WriteContextStringFromString( context, buffer, u'\0' );
}

template<>
void WriteContextString( rftl::basic_string<char32_t> const& context, Utf32LogContextBuffer& buffer )
{
	details::WriteContextStringFromString( context, buffer, U'\0' );
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

void LogVA(
	rftl::u8string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u8string_view format,
	rftl::format_args&& args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::move( args ) );
}



void LogVA(
	rftl::u16string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u16string_view format,
	rftl::format_args&& args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::move( args ) );
}



void LogVA(
	rftl::u32string_view context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u32string_view format,
	rftl::format_args&& args )
{
	GetOrCreateGlobalLoggingInstance().LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::move( args ) );
}

}
///////////////////////////////////////////////////////////////////////////////
}
