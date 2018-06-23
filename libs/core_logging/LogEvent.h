#pragma once
#include "rftl/cstdint"


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

	// Line number may be 0 if not applicable
	size_t mLineNumber;

	// Must be consumed immediately, pointers are not permanent
	char const* mTransientFileString;
	char const* mTransientContextString;
	char const* mTransientMessageFormatString;
};

///////////////////////////////////////////////////////////////////////////////
}}
