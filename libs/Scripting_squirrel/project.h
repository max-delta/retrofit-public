#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef SCRIPTINGSQUIRREL_EXPORTS
#define SCRIPTINGSQUIRREL_API __declspec( dllexport )
#else
#define SCRIPTINGSQUIRREL_API __declspec( dllimport )
#endif
#else
#define SCRIPTINGSQUIRREL_API
#endif

inline constexpr char const RFCAT_SQUIRREL[] = "Squirrel";
inline constexpr uint64_t RFCAT_SQUIRREL_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
