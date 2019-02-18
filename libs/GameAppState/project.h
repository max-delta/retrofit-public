#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMEAPPSTATE_EXPORTS
#define GAMEAPPSTATE_API __declspec( dllexport )
#else
#define GAMEAPPSTATE_API __declspec( dllimport )
#endif
#else
#define GAMEAPPSTATE_API
#endif

constexpr char const* RFCAT_GAMEAPPSTATE = "AppState";
constexpr uint64_t RFCAT_GAMEAPPSTATE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
