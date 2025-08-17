#pragma once
#include "rftl/extension/cstring_view.h"
#include "rftl/cstdint"
#include "rftl/string_view"


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
	rftl::cstring_view mTransientFileString;
	rftl::basic_string_view<CharT> mTransientContextString;
	rftl::basic_string_view<CharT> mTransientMessageFormatString;
};

///////////////////////////////////////////////////////////////////////////////
}
