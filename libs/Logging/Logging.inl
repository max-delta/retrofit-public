#pragma once
#include "Logging.h"


namespace RF::logging::details {
///////////////////////////////////////////////////////////////////////////////

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
