#include "stdafx.h"

#include "UniquePtr.h"

#include "rftl/deque"


namespace RF::rftype::extensions {
///////////////////////////////////////////////////////////////////////////////
namespace uniqueptr_accessor_details {
static size_t const gStableKey = 0;
size_t const* GetStableKey()
{
	return &gStableKey;
}
}
///////////////////////////////////////////////////////////////////////////////
}
