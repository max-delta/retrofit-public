#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMEPIXELPHYSICS_EXPORTS
		#define GAMEPIXELPHYSICS_API __declspec( dllexport )
	#else
		#define GAMEPIXELPHYSICS_API __declspec( dllimport )
	#endif
#else
	#define GAMEPIXELPHYSICS_API
#endif

inline constexpr char const RFCAT_GAMEPIXELPHYSICS[] = "PixelPhys";
inline constexpr uint64_t RFCAT_GAMEPIXELPHYSICS_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
