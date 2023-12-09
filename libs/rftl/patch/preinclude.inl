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
	!defined(RFTL_DISABLE_BAD_MSVC_WARNINGS)
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning(push)
	#pragma warning(disable : 4355) // <future> has invalid use of 'this' ptr
	#pragma warning(disable : 4548) // <future> has some broken macro invocation
#elif \
	defined(_MSC_VER) && \
	( _MSC_VER >= 1922 && _MSC_VER <= 1924 ) && \
	!defined(RFTL_DISABLE_BAD_MSVC_WARNINGS)
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning(push)
	#pragma warning(disable : 4355) // <thread> has invalid use of 'this' ptr
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1925 && _MSC_VER <= 1928 ) && \
	!defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( push )
	#pragma warning( disable : 4355 ) // <thread> has invalid use of 'this' ptr
	#pragma warning( disable : 5204 ) // <thread> has virtuals without destructor
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1929 && _MSC_VER <= 1932 ) && \
	!defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( push )
	#pragma warning( disable : 4355 ) // <thread> has invalid use of 'this' ptr
	#pragma warning( disable : 5204 ) // <thread> has virtuals without destructor
	#pragma warning( disable : 6285 ) // <functional> triggers analysis failure
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1933 && _MSC_VER <= 1936 ) && \
	!defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( push )
	#pragma warning( disable : 5262 ) // <atomic> has implicit fall-through
	#pragma warning( disable : 4355 ) // <thread> has invalid use of 'this' ptr
	#pragma warning( disable : 5204 ) // <thread> has virtuals without destructor
#elif defined( _MSC_VER ) && \
	( _MSC_VER >= 1937 && _MSC_VER <= 1938 ) && \
	!defined( RFTL_DISABLE_BAD_MSVC_WARNINGS )
#define RFTL_DISABLE_BAD_MSVC_WARNINGS
	#pragma warning( push )
	#pragma warning( disable : 4355 ) // <thread> has invalid use of 'this' ptr
	#pragma warning( disable : 5204 ) // <thread> has virtuals without destructor
#endif

#if \
	defined(_MSC_VER) && \
	( _MSC_VER >= 1916 && _MSC_VER <= 1922 ) && \
	_HAS_EXCEPTIONS == 0 && \
	!defined(RFTL_ADD_MISSING_NEW_EXCEPTIONS)
#define RFTL_ADD_MISSING_NEW_EXCEPTIONS
namespace stdext {
	class bad_array_new_length;
}
namespace std {
	using stdext::bad_array_new_length;
}
#endif
