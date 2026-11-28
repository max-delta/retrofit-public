namespace RF::buildstamp {
///////////////////////////////////////////////////////////////////////////////

#ifndef RF_BUILDSTAMP_TYPE
	#define RF_BUILDSTAMP_TYPE "unknown"
#endif
inline constexpr char const kType[] = RF_BUILDSTAMP_TYPE;

#ifndef RF_BUILDSTAMP_VERSION
	#define RF_BUILDSTAMP_VERSION "dev"
#endif
inline constexpr char const kVersion[] = RF_BUILDSTAMP_VERSION;

#ifndef RF_BUILDSTAMP_TIME
	// SUBTLE: This doesn't actually dirty compilation each time, since it
	//  isn't re-evaluated as an input for the build system, which is good
	//  because it won't cause constant rebuilds, but bad because it'll go
	//  stale, so real builds should provide this via proper build machinery
	#define RF_BUILDSTAMP_TIME "" __DATE__ " " __TIME__ ""
#endif
inline constexpr char const kTime[] = RF_BUILDSTAMP_TIME;

#ifndef RF_BUILDSTAMP_SOURCE
	#define RF_BUILDSTAMP_SOURCE "devpc"
#endif
inline constexpr char const kSource[] = RF_BUILDSTAMP_SOURCE;

///////////////////////////////////////////////////////////////////////////////
}
