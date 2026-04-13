#pragma once
#include "AppCommon_GraphicalClient/project.h"

#define RF_MODULE_POINT __declspec( dllexport )

inline constexpr char const RFCAT_STARTUPTEST[] = "StartupTest";
inline constexpr uint64_t RFCAT_STARTUPTEST_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
