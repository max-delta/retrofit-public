#pragma once
#include "Logging.h"


namespace RF::logging::details {
///////////////////////////////////////////////////////////////////////////////

#ifdef RF_TREAT_LOG_ASCII_FORMAT_STRINGS_AS_UTF8
template<typename Context>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char const* format, ... )
{
	Utf8LogContextBuffer contextBuffer = Utf8LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = u8'\0';

	char8_t const* const formatAsUtf8 = reinterpret_cast<char8_t const*>( format );

	va_list args;
	va_start( args, format );
	details::LogVA( &contextBuffer[0], categoryKey, severityMask, filename, lineNumber, formatAsUtf8, args );
	va_end( args );
}
#endif



template<typename Context>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char8_t const* format, ... )
{
	Utf8LogContextBuffer contextBuffer = Utf8LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = u8'\0';

	va_list args;
	va_start( args, format );
	details::LogVA( &contextBuffer[0], categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



template<typename Context>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char16_t const* format, ... )
{
	Utf16LogContextBuffer contextBuffer = Utf16LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = '\0';

	va_list args;
	va_start( args, format );
	details::LogVA( &contextBuffer[0], categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



template<typename Context>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	char const* filename,
	size_t lineNumber,
	char32_t const* format, ... )
{
	Utf32LogContextBuffer contextBuffer = Utf32LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = '\0';

	va_list args;
	va_start( args, format );
	details::LogVA( &contextBuffer[0], categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}

///////////////////////////////////////////////////////////////////////////////
}
