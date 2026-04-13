#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMEINPUT_EXPORTS
#define GAMEINPUT_API __declspec( dllexport )
#else
#define GAMEINPUT_API __declspec( dllimport )
#endif
#else
#define GAMEINPUT_API
#endif

inline constexpr char const RFCAT_GAMEINPUT[] = "Input";
inline constexpr uint64_t RFCAT_GAMEINPUT_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
