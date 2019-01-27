#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef SCHEDULING_EXPORTS
#define SCHEDULING_API __declspec( dllexport )
#else
#define SCHEDULING_API __declspec( dllimport )
#endif
#else
#define SCHEDULING_API
#endif

constexpr char const* RFCAT_SCHEDULING = "Scheduling";
constexpr uint64_t RFCAT_SCHEDULING_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
