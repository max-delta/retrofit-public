#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMESYNC_EXPORTS
#define GAMESYNC_API __declspec( dllexport )
#else
#define GAMESYNC_API __declspec( dllimport )
#endif
#else
#define GAMESYNC_API
#endif

constexpr char const* RFCAT_GAMESYNC = "Sync";
constexpr uint64_t RFCAT_GAMESYNC_SEV_WHITELIST = RF::logging::kDefaultSeverityMask & ~RF::logging::RF_SEV_TRACE;
