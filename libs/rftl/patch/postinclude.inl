// NOTE: Included multiple times
//#pragma once

#if \
	defined(_MSC_VER) && \
	(_MSC_VER == 1913) && \
	defined(_EXPERIMENTAL_FILESYSTEM_) && \
	!defined(RFTL_SHIM_FILESYSTEM)
#define RFTL_SHIM_FILESYSTEM
	namespace std { namespace filesystem {
		using namespace std::experimental::filesystem::v1;
	}}
#endif
