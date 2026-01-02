#ifdef __INTELLISENSE__
	#include "PreConfig.h"
#endif

namespace RF::config::flag {
///////////////////////////////////////////////////////////////////////////////
#define RF_CONFIG_FLAG( MACRO ) ( RF_IS_ALLOWED( MACRO ) ? true : false )

static constexpr bool kAsserts = RF_CONFIG_FLAG( RF_CONFIG_ASSERTS );
static constexpr bool kOncePer = RF_CONFIG_FLAG( RF_CONFIG_ONCEPER );
static constexpr bool kFilenameMacro = RF_CONFIG_FLAG( RF_CONFIG_FILENAME_MACRO );
static constexpr bool kTodoMessages = RF_CONFIG_FLAG( RF_CONFIG_TODO_MESSAGES );
static constexpr bool kLangTodoChecks = RF_CONFIG_FLAG( RF_CONFIG_LANG_TODO_CHECKS );
static constexpr bool kUseStdModule = RF_CONFIG_FLAG( RF_CONFIG_USE_STD_MODULE );
static constexpr bool kLegacyBitMath = RF_CONFIG_FLAG( RF_CONFIG_LEGACY_BIT_MATH );
static constexpr bool kInternalBuildNotice = RF_CONFIG_FLAG( RF_CONFIG_INTERNAL_BUILD_NOTICE );
static constexpr bool kClassInfoDebugNames = RF_CONFIG_FLAG( RF_CONFIG_CLASSINFO_DEBUG_NAMES );
static constexpr bool kInformativeLogging = RF_CONFIG_FLAG( RF_CONFIG_INFORMATIVE_LOGGING );
static constexpr bool kVerboseLogging = RF_CONFIG_FLAG( RF_CONFIG_VERBOSE_LOGGING );
static constexpr bool kTraceLogging = RF_CONFIG_FLAG( RF_CONFIG_TRACE_LOGGING );
static constexpr bool kDevCheats = RF_CONFIG_FLAG( RF_CONFIG_DEV_CHEATS );
static constexpr bool kLongTimeouts = RF_CONFIG_FLAG( RF_CONFIG_LONG_TIMEOUTS );
static constexpr bool kRollbackPerfOptimizations = RF_CONFIG_FLAG( RF_CONFIG_ROLLBACK_PERF_OPTIMIZATIONS );
static constexpr bool kInputDebugAccess = RF_CONFIG_FLAG( RF_CONFIG_INPUT_DEBUG_ACCESS );
static constexpr bool kResourceSaveDiagnostics = RF_CONFIG_FLAG( RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS );
static constexpr bool kResourceLoadDiagnostics = RF_CONFIG_FLAG( RF_CONFIG_RESOURCE_LOAD_DIAGNOSTICS );
static constexpr bool kHookNew = RF_CONFIG_FLAG( RF_CONFIG_HOOK_NEW );

#undef RF_CONFIG_FLAG
///////////////////////////////////////////////////////////////////////////////
}
