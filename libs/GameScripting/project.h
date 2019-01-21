#pragma once
#include "Logging/Constants.h"

#ifdef GAMESCRIPTING_EXPORTS
#define GAMESCRIPTING_API __declspec( dllexport )
#else
#define GAMESCRIPTING_API __declspec( dllimport )
#endif

constexpr char const* RFCAT_GAMESCRIPTING = "Scripting";
constexpr uint64_t RFCAT_GAMESCRIPTING_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
