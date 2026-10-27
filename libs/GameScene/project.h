#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMESCENE_EXPORTS
		#define GAMESCENE_API __declspec( dllexport )
	#else
		#define GAMESCENE_API __declspec( dllimport )
	#endif
#else
	#define GAMESCENE_API
#endif

inline constexpr char const RFCAT_GAMESCENE[] = "Scene";
inline constexpr uint64_t RFCAT_GAMESCENE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
