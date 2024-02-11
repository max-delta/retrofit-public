#pragma once
#include "core_pe/PeFwd.h"

#include "rftl/iosfwd"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

struct DosHeader
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	size_t mAbsoluteOffsetToPEHeader = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
