#pragma once
#include "Logging/Constants.h"

#ifdef SCHEDULING_EXPORTS
#define SCHEDULING_API __declspec(dllexport)
#else
#define SCHEDULING_API __declspec(dllimport)
#endif

constexpr char const* RFCAT_SCHEDULING = "Scheduling";
constexpr uint64_t RFCAT_SCHEDULING_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
