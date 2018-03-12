#pragma once
#include <stdint.h>


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

using CategoryKey = char const*;
using SeverityMask = uint64_t;

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

///////////////////////////////////////////////////////////////////////////////
}}
