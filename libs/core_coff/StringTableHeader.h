#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/iosfwd"
#include "rftl/vector"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct StringTableHeader
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );
	rftl::vector<uint8_t> TryCopyBytes( rftl::streambuf& seekable, size_t seekBase ) const;

public:
	size_t mSize = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
