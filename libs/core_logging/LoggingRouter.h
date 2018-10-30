#pragma once
#include "core_logging/LoggingHandler.h"
#include "core_math/Hash.h"
#include "core/macros.h"

#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/shared_mutex"
#include "rftl/array"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

class LoggingRouter
{
	RF_NO_COPY( LoggingRouter );

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
	LoggingRouter( LoggingRouter && ) = default;
	LoggingRouter& operator=( LoggingRouter && ) = default;

	void Log(
		char const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char const* format, ... ) const;
	void Log(
		char16_t const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char16_t const* format, ... ) const;
	void Log(
		char32_t const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char32_t const* format, ... ) const;
	void LogVA(
		char const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char const* format,
		va_list args ) const;
	void LogVA(
		char16_t const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char16_t const* format,
		va_list args ) const;
	void LogVA(
		char32_t const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		char32_t const* format,
		va_list args ) const;

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
	void SetOrModifyGlobalWhitelist(SeverityMask whitelist);
	void ClearGlobalWhitelist();
	void SetOrModifyCategoryWhitelist( CategoryKey categoryKey, SeverityMask whitelist );
	void ClearCategoryWhitelist( CategoryKey categoryKey);


	//
	// Private methods
private:
	template<typename CharT>
	void LogInternal(
		CharT const* context,
		CategoryKey categoryKey,
		SeverityMask severityMask,
		char const* filename,
		size_t lineNumber,
		CharT const* format,
		va_list args ) const;
	void LogInternal( LogEvent<char> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const;
	void LogInternal( LogEvent<char16_t> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const;
	void LogInternal( LogEvent<char32_t> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const;
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
}}
