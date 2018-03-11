#pragma once
#include "core/macros.h"

#include <vector>
#include <shared_mutex>
#include <array>
#include <stdint.h>


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

class LoggingRouter
{
	RF_NO_COPY( LoggingRouter );

	//
	// Forwards
public:
	struct LogEvent;
	struct HandlerDefinition;


	//
	// Types and constants
public:
	using CategoryKey = char const*;
	using SeverityMask = uint64_t;
	using SeverityStrings = std::array<char const*, 64>;
	using HandlerID = uint64_t;
	static constexpr HandlerID kInvalidHandlerID = 0;
	using HandlerFunc = void(*)( LoggingRouter const&, LogEvent const&, va_list args );
private:
	static constexpr HandlerID kInitialHandlerID = kInvalidHandlerID + 1;
	using ReaderWriterMutex = std::shared_mutex;
	using ReaderLock = std::shared_lock<std::shared_mutex>;
	using WriterLock = std::unique_lock<std::shared_mutex>;
	using HandlerDefinitionByID = std::pair<HandlerID, HandlerDefinition>;
	using HandlerDefinitionsByPriority = std::vector<HandlerDefinitionByID>;


	//
	// Structs
public:
	struct LogEvent
	{
		// Values not gauranteed to be consistent across builds
		CategoryKey mCategoryKey;

		// May have multiple severities set
		SeverityMask mSeverityMask;

		// Must be consumed immediately, pointers are not permanent
		char const* mTransientContextString;
		char const* mTransientMessageFormatString;
	};

	struct HandlerDefinition
	{
		// May recieve additional severities on multi-severity events
		SeverityMask mSupportedSeverities;

		// Must support multi-threading
		HandlerFunc mHandlerFunc;
	};


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
