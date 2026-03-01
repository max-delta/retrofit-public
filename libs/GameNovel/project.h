#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMENOVEL_EXPORTS
		#define GAMENOVEL_API __declspec( dllexport )
	#else
		#define GAMENOVEL_API __declspec( dllimport )
	#endif
#else
	#define GAMENOVEL_API
#endif

constexpr char const* RFCAT_GAMENOVEL = "Novel";
constexpr uint64_t RFCAT_GAMENOVEL_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
