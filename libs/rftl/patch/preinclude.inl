// NOTE: Included multiple times
//#pragma once

#include "normallocations.inl"

#if \
	defined(_MSC_VER) && \
	(_MSC_VER == 1913) && \
	!defined(RFTL_OVERRIDE_FILESYSTEM)
#define RFTL_OVERRIDE_FILESYSTEM
	#undef RFTL_FILESYSTEM
	#define RFTL_FILESYSTEM <experimental/filesystem>
#endif
