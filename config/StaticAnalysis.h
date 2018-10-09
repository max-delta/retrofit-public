#include <CppCoreCheck/warnings.h>

// In general, these warnings are full of trash, and even selective
//  whitelisting isn't viable due to bugs relating to static analysis and
//  the (at time of writing) experimental third-party warning disabling
//  feature in MSVC
// TODO: Review once these features are less experimental
#pragma warning(disable:ALL_CPPCORECHECK_WARNINGS)

// This version of MSVC doesn't include this warning in <CppCoreCheck/warnings.h>
#if _MSC_FULL_VER >= 191526726 && _MSC_FULL_VER <= 191526730
	#pragma warning(disable:26455)
#endif
