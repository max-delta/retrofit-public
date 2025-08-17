#pragma once
#include "core_logging/LoggingHandler.h"
#include "core_math/Hash.h"
#include "core/macros.h"

#include "rftl/array"
#include "rftl/shared_mutex"
#include "rftl/unordered_map"
#include "rftl/vector"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

class LoggingRouter
{
	RF_NO_COPY( LoggingRouter );
	RF_NO_MOVE( LoggingRouter );

	//
	// Types and constants
public:
	using SeverityStrings = rftl::array<char const*, 64>;
private:
	static constexpr HandlerID kInitialHandlerID = kInvalidHandlerID + 1;
	static constexpr SeverityMask kDefaultGlobalWhitelist = ~SeverityMask( 0 );
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;
	using HandlerDefinitionByID = rftl::pair<HandlerID, HandlerDefinition>;
	using HandlerDefinitionsByPriority = rftl::vector<HandlerDefinitionByID>;
	using SeverityMasksByCategoryKey = rftl::unordered_map<CategoryKey, SeverityMask, math::NullTerminatedStringHash>;


	//
	// Public methods
public:
	LoggingRouter();

	template<typename... ArgsT>
	void Log(
		rftl::u8string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u8string_view format,
		ArgsT&&... args ) const
	{
		LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
	}
	template<typename... ArgsT>
	void Log(
		rftl::u16string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u16string_view format,
		ArgsT&&... args ) const
	{
		LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
	}
	template<typename... ArgsT>
	void Log(
		rftl::u32string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u32string_view format,
		ArgsT&&... args ) const
	{
		LogVA( context, categoryKey, severityMask, filename, lineNumber, format, rftl::make_format_args( args... ) );
	}
	void LogVA(
		rftl::u8string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u8string_view format,
		rftl::format_args&& args ) const;
	void LogVA(
		rftl::u16string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u16string_view format,
		rftl::format_args&& args ) const;
	void LogVA(
		rftl::u32string_view context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::u32string_view format,
		rftl::format_args&& args ) const;

	// Control which handlers are able to handle log events
	// NOTE: Handlers are run in the order they are registered
	HandlerID RegisterHandler( HandlerDefinition const& handlerDefinition );
	bool UnregisterHandler( HandlerID handlerID );

	char const* GetStringFromCategoryKey( CategoryKey categoryKey ) const;

	// Severity bits can have names, defined by immutable strings
	// NOTE: This means that for practical purposes these strings must be
	//  literals whose memory is tied to the lifetime of the executable
	void SetSeverityString( SeverityMask singleBit, char const* immutableString );
	SeverityStrings GetSeverityStrings() const;

	// Dynamic whitelists
	void SetOrModifyGlobalWhitelist( SeverityMask whitelist );
	void ClearGlobalWhitelist();
	void SetOrModifyCategoryWhitelist( CategoryKey categoryKey, SeverityMask whitelist );
	void ClearCategoryWhitelist( CategoryKey categoryKey );


	//
	// Private methods
private:
	template<typename CharT>
	void LogInternal(
		rftl::basic_string_view<CharT> context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		rftl::cstring_view filename,
		size_t lineNumber,
		rftl::basic_string_view<CharT> format,
		rftl::format_args&& args ) const;
	void Dispatch( LogEvent<char8_t> const& logEvent, HandlerDefinition const& handlerDef, rftl::format_args const& args ) const;
	void Dispatch( LogEvent<char16_t> const& logEvent, HandlerDefinition const& handlerDef, rftl::format_args const& args ) const;
	void Dispatch( LogEvent<char32_t> const& logEvent, HandlerDefinition const& handlerDef, rftl::format_args const& args ) const;
	bool IsDynamicallyFilteredOut( CategoryKey categoryKey, SeverityMask severityMask ) const;


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;

	HandlerID mHandlerIDGenerator;
	HandlerDefinitionsByPriority mHandlerDefinitionsByPriority;

	SeverityStrings mSeverityStrings;

	SeverityMask mDynamicGlobalWhitelist;
	SeverityMasksByCategoryKey mDynamicCategoryWhitelists;
};

///////////////////////////////////////////////////////////////////////////////
}
