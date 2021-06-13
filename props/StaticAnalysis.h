#include <CppCoreCheck/warnings.h>

// In general, these warnings are full of trash, and even selective
//  whitelisting isn't viable due to bugs relating to static analysis and
//  the (at time of writing) experimental third-party warning disabling
//  feature in MSVC
// TODO: Review once these features are less experimental
#pragma warning( disable : ALL_CPPCORECHECK_WARNINGS )

// This version of MSVC doesn't include this warning in <CppCoreCheck/warnings.h>
#if _MSC_FULL_VER >= 191526726 && _MSC_FULL_VER <= 191526732
	#pragma warning( disable : 26455 )
#endif
#if _MSC_FULL_VER >= 192829910 && _MSC_FULL_VER <= 192930037
	#pragma warning( disable : 26457 )
#endif
#if _MSC_FULL_VER >= 192930037 && _MSC_FULL_VER <= 192930037
	#pragma warning( disable : 26467 )
#endif

// False positives about locks being dropped in non-sensical locations
// NOTE: Specifically observed with rftl::lock_guard<rftl::mutex> not
//  understanding where scope ends, and thinking the destructor would happen
//  in the same line as the constructor
#if _MSC_FULL_VER >= 192729111 && _MSC_FULL_VER <= 192930037
	#pragma warning( disable : 26110 )
#endif
