#pragma once
#include "rftl/cstdint"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

using CategoryKey = char const*;
using SeverityMask = uint64_t;

template<typename CharT>
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
	CharT const* mTransientContextString;
	CharT const* mTransientMessageFormatString;
};

///////////////////////////////////////////////////////////////////////////////
}
