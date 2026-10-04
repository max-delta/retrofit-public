#pragma once
#include "rftl/cstdint"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

using CategoryKey = char const*;
using SeverityMask = uint64_t;

using HandlerID = uint64_t;
inline constexpr HandlerID kInvalidHandlerID = 0;

using HandlerPriority = uint8_t;
inline constexpr HandlerPriority kInvalidHandlerPriority = 0;
inline constexpr HandlerPriority kEarliestHandlerPriority = 1;
inline constexpr HandlerPriority kLatestHandlerPriority = 255;

template<typename CharT>
struct LogEvent;

class LoggingRouter;

///////////////////////////////////////////////////////////////////////////////
}
