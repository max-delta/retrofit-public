#pragma once
#include "AppCommon_GraphicalClient/project.h"

#define RF_MODULE_POINT __declspec( dllexport )

inline constexpr char const RFCAT_CC3O3[] = "CC3O3";
inline constexpr uint64_t RFCAT_CC3O3_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;

inline constexpr char const RFCAT_CHAR[] = "Character";
inline constexpr uint64_t RFCAT_CHAR_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
