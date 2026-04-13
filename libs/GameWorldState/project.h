#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMEWORLDSTATE_EXPORTS
#define GAMEWORLDSTATE_API __declspec( dllexport )
#else
#define GAMEWORLDSTATE_API __declspec( dllimport )
#endif
#else
#define GAMEWORLDSTATE_API
#endif

inline constexpr char const RFCAT_GAMEWORLDSTATE[] = "WorldState";
inline constexpr uint64_t RFCAT_GAMEWORLDSTATE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
