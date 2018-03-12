#pragma once
#include "core_logging/LogEvent.h"


// Forwards
namespace RF { namespace logging {
	class LoggingRouter;
}}

namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

using HandlerID = uint64_t;
static constexpr HandlerID kInvalidHandlerID = 0;
using HandlerFunc = void(*)( LoggingRouter const&, LogEvent const&, va_list args );

struct HandlerDefinition
{
	// May recieve additional severities on multi-severity events
	SeverityMask mSupportedSeverities;

	// Must support multi-threading
	HandlerFunc mHandlerFunc;
};

///////////////////////////////////////////////////////////////////////////////
}}
