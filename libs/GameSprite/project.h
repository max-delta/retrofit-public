#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMESPRITE_EXPORTS
#define GAMESPRITE_API __declspec( dllexport )
#else
#define GAMESPRITE_API __declspec( dllimport )
#endif
#else
#define GAMESPRITE_API
#endif

inline constexpr char const RFCAT_GAMESPRITE[] = "Sprite";
inline constexpr uint64_t RFCAT_GAMESPRITE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
