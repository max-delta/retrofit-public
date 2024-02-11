#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/chrono"
#include "rftl/iosfwd"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct CoffHeader
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	uint16_t mMachineType = 0;
	size_t mNumSections = 0;
	size_t mAbsoluteOffsetToSymbolTable = 0;
	rftl::chrono::seconds mTimeSinceUnixEpoch = {};
	size_t mNumSymbols = 0;
	size_t mOptionalHeaderBytes = 0;
	uint16_t mFlags = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
