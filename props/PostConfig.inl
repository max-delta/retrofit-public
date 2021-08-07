#ifdef __INTELLISENSE__
	#include "PreConfig.h"
#endif

namespace RF::config {
///////////////////////////////////////////////////////////////////////////////
#define RF_CONFIG_FLAG( MACRO ) ( RF_IS_ALLOWED( MACRO ) ? true : false )

static constexpr bool kAsserts = RF_CONFIG_FLAG( RF_CONFIG_ASSERTS );
static constexpr bool kOncePer = RF_CONFIG_FLAG( RF_CONFIG_ONCEPER );
static constexpr bool kFilenameMacro = RF_CONFIG_FLAG( RF_CONFIG_FILENAME_MACRO );
static constexpr bool kTodoMessages = RF_CONFIG_FLAG( RF_CONFIG_TODO_MESSAGES );
static constexpr bool kInternalBuildNotice = RF_CONFIG_FLAG( RF_CONFIG_INTERNAL_BUILD_NOTICE );
static constexpr bool kDevCheats = RF_CONFIG_FLAG( RF_CONFIG_DEV_CHEATS );
static constexpr bool kLongTimeouts = RF_CONFIG_FLAG( RF_CONFIG_LONG_TIMEOUTS );
static constexpr bool kRollbackPerfOptimizations = RF_CONFIG_FLAG( RF_CONFIG_ROLLBACK_PERF_OPTIMIZATIONS );
static constexpr bool kHookNew = RF_CONFIG_FLAG( RF_CONFIG_HOOK_NEW );

#undef RF_CONFIG_FLAG
///////////////////////////////////////////////////////////////////////////////
}
