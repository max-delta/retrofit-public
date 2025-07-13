#include "PreConfig.h"

// Debug builds are for local iteration only. Not for internal or public
//  release. Likely have bad performance and contain secrets.

#define RF_CONFIG_ASSERTS RF_ALLOW
#define RF_CONFIG_ONCEPER RF_ALLOW
#define RF_CONFIG_FILENAME_MACRO RF_ALLOW

// Forbid: Don't need to see this while iterating, just clogs up output
#define RF_CONFIG_TODO_MESSAGES RF_FORBID

#define RF_CONFIG_LANG_TODO_CHECKS RF_FORBID

// C++20 adds modules, but only C++23 adds the standard library to modules...
#define RF_CONFIG_USE_STD_MODULE RF_FORBID

// Old math code from before C++20 <bit>
#define RF_CONFIG_LEGACY_BIT_MATH RF_FORBID

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_ALLOW
#define RF_CONFIG_CLASSINFO_DEBUG_NAMES RF_ALLOW

#define RF_CONFIG_DEV_CHEATS RF_ALLOW
#define RF_CONFIG_LONG_TIMEOUTS RF_ALLOW
#define RF_CONFIG_ROLLBACK_PERF_OPTIMIZATIONS RF_FORBID

#define RF_CONFIG_INPUT_DEBUG_ACCESS RF_ALLOW

#define RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS RF_ALLOW
#define RF_CONFIG_RESOURCE_LOAD_DIAGNOSTICS RF_ALLOW

// Override new
#define RF_CONFIG_HOOK_NEW RF_ALLOW

#include "PostConfig.inl"
