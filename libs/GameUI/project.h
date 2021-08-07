#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMEUI_EXPORTS
#define GAMEUI_API __declspec( dllexport )
#else
#define GAMEUI_API __declspec( dllimport )
#endif
#else
#define GAMEUI_API
#endif

constexpr char const* RFCAT_GAMEUI = "UI";
constexpr uint64_t RFCAT_GAMEUI_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
