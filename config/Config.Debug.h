#include "Config.h"

#if __has_include( "__BuildStamp.h" )
	#include "__BuildStamp.h"
#else
	#define RF_BUILDSTAMP_TYPE "debug"
	#define RF_BUILDSTAMP_VERSION "dev"
	#define RF_BUILDSTAMP_TIME "" __DATE__ " " __TIME__ ""
	#define RF_BUILDSTAMP_SOURCE "devpc"
#endif

#define RF_CONFIG_ASSERTS RF_ALLOW
#define RF_CONFIG_FILENAME_MACRO RF_ALLOW

// Forbid: Don't need to see this while iterating, just clogs up output
#define RF_CONFIG_TODO_MESSAGES RF_FORBID

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_ALLOW
