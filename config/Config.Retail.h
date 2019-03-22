#include "Config.h"

#if __has_include( "__BuildStamp.h" )
	#include "__BuildStamp.h"
#else
	#define RF_BUILDSTAMP_TYPE "retail"
	#define RF_BUILDSTAMP_VERSION "dev"
	#define RF_BUILDSTAMP_TIME "" __DATE__ " " __TIME__ ""
	#define RF_BUILDSTAMP_SOURCE "devpc"
#endif

#define RF_CONFIG_ASSERTS RF_FORBID
#define RF_CONFIG_FILENAME_MACRO RF_FORBID

#define RF_CONFIG_INTERNAL_BUILD_NOTICE RF_FORBID
