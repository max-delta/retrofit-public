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

#if \
	defined(_MSC_VER) && \
	(_MSC_VER >= 1913 && _MSC_VER <= 1916) && \
	!defined(RFTL_DISBALE_BAD_MSVC_WARNINGS)
#define RFTL_DISBALE_BAD_MSVC_WARNINGS
	#pragma warning(push)
	#pragma warning(disable : 4355) // <future> has invalid use of 'this' ptr
	#pragma warning(disable : 4548) // <future> has some broken macro invocation
#endif
