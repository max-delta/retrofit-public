#include "PreConfig.h"

// Release builds are for internal development. Not for public release. Should
//  have decent performance, but may contain secrets.

#define RF_CONFIG_ASSERTS RF_FORBID
#define RF_CONFIG_ONCEPER RF_ALLOW
#define RF_CONFIG_FILENAME_MACRO RF_ALLOW

// Allow: Testing and internal releases are a good time to evaluate TODOs
#define RF_CONFIG_TODO_MESSAGES RF_ALLOW

#define RF_CONFIG_LANG_TODO_CHECKS RF_FORBID

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_ALLOW
#define RF_CONFIG_CLASSINFO_DEBUG_NAMES RF_ALLOW

#define RF_CONFIG_DEV_CHEATS RF_ALLOW
#define RF_CONFIG_LONG_TIMEOUTS RF_ALLOW
#define RF_CONFIG_ROLLBACK_PERF_OPTIMIZATIONS RF_ALLOW

#define RF_CONFIG_INPUT_DEBUG_ACCESS RF_ALLOW

#define RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS RF_FORBID
#define RF_CONFIG_RESOURCE_LOAD_DIAGNOSTICS RF_FORBID

// Override new
#define RF_CONFIG_HOOK_NEW RF_ALLOW

#include "PostConfig.inl"
