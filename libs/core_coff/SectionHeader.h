#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/extension/static_string.h"
#include "rftl/iosfwd"
#include "rftl/optional"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct SectionHeader
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	rftl::static_string<8> mName = {};
	size_t mMemoryBytes = 0;
	size_t mAbsoluteOffsetToMemoryAddress = 0;
	size_t mRawDataFileBytes = 0;
	size_t mAbsoluteOffsetToAddress = 0;
	size_t mAbsoluteOffsetToRelocations = 0;
	size_t mAbsoluteOffsetToLineNumbers = 0;
	size_t mNumRelocations = 0;
	size_t mNumLineNumbers = 0;
	uint32_t mFlags = 0;
	size_t mRelativeOffsetToNextSection = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
