#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef SERIALIZATION_EXPORTS
#define SERIALIZATION_API __declspec( dllexport )
#else
#define SERIALIZATION_API __declspec( dllimport )
#endif
#else
#define SERIALIZATION_API
#endif

constexpr char const* RFCAT_SERIALIZATION = "Serialization";
constexpr uint64_t RFCAT_SERIALIZATION_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
