#pragma once
#include "Logging/Constants.h"

#ifdef PLATFORMUTILS_EXPORTS
	#define PLATFORMUTILS_API __declspec(dllexport)
#else
	#define PLATFORMUTILS_API __declspec(dllimport)
#endif

constexpr char const* RFCAT_PLATFORMUTILS = "PlatformUtils";
constexpr uint64_t RFCAT_PLATFORMUTILS_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
