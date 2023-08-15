// NOTE: Included multiple times
//#pragma once

#if \
	defined(_MSC_VER) && \
	(_MSC_VER == 1913) && \
	defined(_EXPERIMENTAL_FILESYSTEM_) && \
	!defined(RFTL_SHIM_FILESYSTEM)
#define RFTL_SHIM_FILESYSTEM
	namespace std::filesystem {
		using namespace std::experimental::filesystem::v1;
	}}
#endif

#if \
	defined(_MSC_VER) && \
	(_MSC_VER >= 1913 && _MSC_VER <= 1916) && \
	defined(RFTL_DISABLE_BAD_MSVC_WARNINGS)
#undef RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning(suppress : 5031) // 'push' came from different file
	#pragma warning(pop)
#elif \
	defined(_MSC_VER) && \
	( _MSC_VER >= 1922 && _MSC_VER <= 1932 ) && \
	defined(RFTL_DISABLE_BAD_MSVC_WARNINGS)
#undef RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning(suppress : 5031) // 'push' came from different file
	#pragma warning(pop)
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1933 && _MSC_VER <= 1936 ) && \
	defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#undef RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( suppress : 5031 ) // 'push' came from different file
	#pragma warning( pop )
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1937 && _MSC_VER <= 1937 ) && \
	defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#undef RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( suppress : 5031 ) // 'push' came from different file
	#pragma warning( pop )
#endif
