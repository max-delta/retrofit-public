#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef LOCALIZATION_EXPORTS
#define LOCALIZATION_API __declspec( dllexport )
#else
#define LOCALIZATION_API __declspec( dllimport )
#endif
#else
#define LOCALIZATION_API
#endif

constexpr char const* RFCAT_LOCALIZATION = "Loc";
constexpr uint64_t RFCAT_LOCALIZATION_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
