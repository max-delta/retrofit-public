#pragma once
#include "Logging/Constants.h"

#ifdef PPU_EXPORTS
#define PPU_API __declspec( dllexport )
#else
#define PPU_API __declspec( dllimport )
#endif

constexpr char const* RFCAT_PPU = "PPU";
constexpr uint64_t RFCAT_PPU_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
