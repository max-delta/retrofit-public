#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef PLATFORMNETWORK_EXPORTS
		#define PLATFORMNETWORK_API __declspec( dllexport )
	#else
		#define PLATFORMNETWORK_API __declspec( dllimport )
	#endif
#else
	#define PLATFORMNETWORK_API
#endif

constexpr char const* RFCAT_PLATFORMNETWORK = "PlatformNetwork";
constexpr uint64_t RFCAT_PLATFORMNETWORK_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
