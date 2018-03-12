#pragma once
#include "core_logging/LoggingHandler.h"
#include "core/macros.h"

#include <vector>
#include <shared_mutex>
#include <array>


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

class LoggingRouter
{
	RF_NO_COPY( LoggingRouter );

	//
	// Types and constants
public:
	using SeverityStrings = std::array<char const*, 64>;
private:
	static constexpr HandlerID kInitialHandlerID = kInvalidHandlerID + 1;
	using ReaderWriterMutex = std::shared_mutex;
	using ReaderLock = std::shared_lock<std::shared_mutex>;
	using WriterLock = std::unique_lock<std::shared_mutex>;
	using HandlerDefinitionByID = std::pair<HandlerID, HandlerDefinition>;
	using HandlerDefinitionsByPriority = std::vector<HandlerDefinitionByID>;


	//
	// Public methods
public:
	LoggingRouter();
	LoggingRouter( LoggingRouter && ) = default;
	LoggingRouter& operator=( LoggingRouter && ) = default;

	void Log( char const* context, CategoryKey categoryKey, SeverityMask severityMask, char const* format, ... ) const;
	void LogVA( char const* context, CategoryKey categoryKey, SeverityMask severityMask, char const* format, va_list args ) const;

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


	//
	// Private methods
private:
	void LogInternal( char const* context, CategoryKey categoryKey, SeverityMask severityMask, char const* format, va_list args ) const;


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	HandlerID mHandlerIDGenerator;
	HandlerDefinitionsByPriority mHandlerDefinitionsByPriority;
	SeverityStrings mSeverityStrings;
};

///////////////////////////////////////////////////////////////////////////////
}}
