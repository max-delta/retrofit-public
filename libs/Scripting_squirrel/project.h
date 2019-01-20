#pragma once
#include "Logging/Constants.h"

#ifdef SCRIPTINGSQUIRREL_EXPORTS
#define SCRIPTINGSQUIRREL_API __declspec( dllexport )
#else
#define SCRIPTINGSQUIRREL_API __declspec( dllimport )
#endif

constexpr char const* RFCAT_SQUIRREL = "Squirrel";
constexpr uint64_t RFCAT_SQUIRREL_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
