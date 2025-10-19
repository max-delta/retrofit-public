#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMEDIALOGUE_EXPORTS
		#define GAMEDIALOGUE_API __declspec( dllexport )
	#else
		#define GAMEDIALOGUE_API __declspec( dllimport )
	#endif
#else
	#define GAMEDIALOGUE_API
#endif

constexpr char const* RFCAT_GAMEDIALOGUE = "Dialogue";
constexpr uint64_t RFCAT_GAMEDIALOGUE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
