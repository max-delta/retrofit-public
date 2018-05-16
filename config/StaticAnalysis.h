#include <CppCoreCheck/warnings.h>

// In general, these warnings are full of trash, and even selective
//  whitelisting isn't viable due to bugs relating to static analysis and
//  the (at time of writing) experimental third-party warning disabling
//  feature in MSVC
// TODO: Review once these features are less experimental
#pragma warning(disable:ALL_CPPCORECHECK_WARNINGS)
