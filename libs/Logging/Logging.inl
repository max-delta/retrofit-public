#pragma once
#include "Logging.h"


namespace RF::logging::details {
///////////////////////////////////////////////////////////////////////////////

#ifdef RF_TREAT_LOG_ASCII_FORMAT_STRINGS_AS_UTF8
template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::string_view format,
	ArgsT&&... args )
{
	Utf8LogContextBuffer contextBuffer = Utf8LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = u8'\0';

	rftl::u8string_view const formatAsUtf8 = rftl::u8string_view( reinterpret_cast<char8_t const*>( format.data() ), format.size() );

	details::LogVA( contextBuffer.data(), categoryKey, severityMask, filename, lineNumber, formatAsUtf8, rftl::make_format_args( args... ) );
}
#endif



template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u8string_view format,
	ArgsT&&... args )
{
	Utf8LogContextBuffer contextBuffer = Utf8LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = u8'\0';

	details::LogVA( contextBuffer.data(), categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}



template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u16string_view format,
	ArgsT&&... args )
{
	Utf16LogContextBuffer contextBuffer = Utf16LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = '\0';

	details::LogVA( contextBuffer.data(), categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}



template<typename Context, typename... ArgsT>
void Log(
	Context const& context,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u32string_view format,
	ArgsT&&... args )
{
	Utf32LogContextBuffer contextBuffer = Utf32LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = '\0';

	details::LogVA( contextBuffer.data(), categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}



template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::string_view format,
	ArgsT&&... args )
{
	rftl::u8string_view const formatAsUtf8 = rftl::u8string_view( reinterpret_cast<char8_t const*>( format.data() ), format.size() );

	details::LogVA( {}, categoryKey, severityMask, filename, lineNumber, formatAsUtf8, rftl::make_format_args( args... ) );
}



template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u8string_view format,
	ArgsT&&... args )
{
	details::LogVA( {}, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}

template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u16string_view format,
	ArgsT&&... args )
{
	details::LogVA( {}, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}

template<typename... ArgsT>
void Log(
	nullptr_t /*context*/,
	CategoryKey categoryKey,
	uint64_t severityMask,
	rftl::cstring_view filename,
	size_t lineNumber,
	rftl::u32string_view format,
	ArgsT&&... args )
{
	details::LogVA( {}, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
}

///////////////////////////////////////////////////////////////////////////////
}
