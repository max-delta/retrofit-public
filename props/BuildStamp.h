namespace RF::buildstamp {
///////////////////////////////////////////////////////////////////////////////

#ifndef RF_BUILDSTAMP_TYPE
	#define RF_BUILDSTAMP_TYPE "unknown"
#endif
static constexpr char const kType[] = RF_BUILDSTAMP_TYPE;

#ifndef RF_BUILDSTAMP_VERSION
	#define RF_BUILDSTAMP_VERSION "dev"
#endif
static constexpr char const kVersion[] = RF_BUILDSTAMP_VERSION;

#ifndef RF_BUILDSTAMP_TIME
	#define RF_BUILDSTAMP_TIME "" __DATE__ " " __TIME__ ""
#endif
static constexpr char const kTime[] = RF_BUILDSTAMP_TIME;

#ifndef RF_BUILDSTAMP_SOURCE
	#define RF_BUILDSTAMP_SOURCE "devpc"
#endif
static constexpr char const kSource[] = RF_BUILDSTAMP_SOURCE;

///////////////////////////////////////////////////////////////////////////////
}
