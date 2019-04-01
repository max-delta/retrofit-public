#include "Config.h"

// Release builds are for internal development. Not for public release. Should
//  have decent performance, but may contain secrets.

#if __has_include( "__BuildStamp.h" )
	#include "__BuildStamp.h"
#else
	#define RF_BUILDSTAMP_TYPE "release"
	#define RF_BUILDSTAMP_VERSION "dev"
	#define RF_BUILDSTAMP_TIME "" __DATE__ " " __TIME__ ""
	#define RF_BUILDSTAMP_SOURCE "devpc"
#endif

#define RF_CONFIG_ASSERTS RF_FORBID
#define RF_CONFIG_ONCEPER RF_ALLOW
#define RF_CONFIG_FILENAME_MACRO RF_ALLOW

// Allow: Testing and internal releases are a good time to evaluate TODOs
#define RF_CONFIG_TODO_MESSAGES RF_ALLOW

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_ALLOW

// Override new
#define RF_CONFIG_HOOK_NEW RF_ALLOW
