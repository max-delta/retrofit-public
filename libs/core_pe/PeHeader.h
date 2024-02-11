#pragma once
#include "core_pe/PeFwd.h"

#include "rftl/iosfwd"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

struct PeHeader
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	size_t mRelativeOffsetToCOFFHeader = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
