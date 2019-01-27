#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef PLATFORMUTILS_EXPORTS
#define PLATFORMUTILS_API __declspec( dllexport )
#else
#define PLATFORMUTILS_API __declspec( dllimport )
#endif
#else
#define PLATFORMUTILS_API
#endif

constexpr char const* RFCAT_PLATFORMUTILS = "PlatformUtils";
constexpr uint64_t RFCAT_PLATFORMUTILS_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
