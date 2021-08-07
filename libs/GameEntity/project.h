#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef GAMEENTITY_EXPORTS
#define GAMEENTITY_API __declspec( dllexport )
#else
#define GAMEENTITY_API __declspec( dllimport )
#endif
#else
#define GAMEENTITY_API
#endif

constexpr char const* RFCAT_GAMEENTITY = "Entity";
constexpr uint64_t RFCAT_GAMEENTITY_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
