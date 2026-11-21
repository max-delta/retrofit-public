#pragma once
#include "project.h"

#include "rftl/chrono"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

// Override how time is reported
// NOTE: Thread-safe, but relaxed timing
using TimingSourceSig = rftl::chrono::nanoseconds();
LOGGING_API TimingSourceSig* ReplaceTimingSource( TimingSourceSig* callback );

//////////////////////////////////////////////////////////////////////////////
namespace details {

// NOTE: Thread-safe, but relaxed timing
LOGGING_API rftl::chrono::nanoseconds GetLoggerTime();

}
///////////////////////////////////////////////////////////////////////////////
}
