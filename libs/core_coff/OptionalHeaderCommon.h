#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/iosfwd"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct OptionalHeaderCommon
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	uint16_t mMagic = 0;
	uint16_t mVersion = 0;
	size_t mCodeBytes = 0;
	size_t mInitializedBytes = 0;
	size_t mUninitializedBytes = 0;
	size_t mAbsoluteOffsetToEntryPoint = 0;
	size_t mAbsoluteOffsetToCode = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
