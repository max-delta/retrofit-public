#include "PreConfig.h"

// Retail builds are for public release. Should have optimal performance and
//  contain no secrets. Should be string-scanned before release to verify.

#define RF_CONFIG_ASSERTS RF_FORBID
#define RF_CONFIG_ONCEPER RF_FORBID
#define RF_CONFIG_FILENAME_MACRO RF_FORBID

// Forbid: If it's not done by the retail build, it's probably not getting done
#define RF_CONFIG_TODO_MESSAGES RF_FORBID

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_FORBID

#define RF_CONFIG_DEV_CHEATS RF_FORBID
#define RF_CONFIG_LONG_TIMEOUTS RF_FORBID

// Override new
#define RF_CONFIG_HOOK_NEW RF_FORBID

#include "PostConfig.inl"
