#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef PPU_EXPORTS
#define PPU_API __declspec( dllexport )
#else
#define PPU_API __declspec( dllimport )
#endif
#else
#define PPU_API
#endif

constexpr char const* RFCAT_PPU = "PPU";
constexpr uint64_t RFCAT_PPU_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
