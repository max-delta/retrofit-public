#pragma once
#include "rftl/cstdint"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

using CategoryKey = char const*;
using SeverityMask = uint64_t;

using HandlerID = uint64_t;
static constexpr HandlerID kInvalidHandlerID = 0;

template<typename CharT>
struct LogEvent;

class LoggingRouter;

///////////////////////////////////////////////////////////////////////////////
}
