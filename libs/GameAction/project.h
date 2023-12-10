#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMEACTION_EXPORTS
		#define GAMEACTION_API __declspec( dllexport )
	#else
		#define GAMEACTION_API __declspec( dllimport )
	#endif
#else
	#define GAMEACTION_API
#endif

constexpr char const* RFCAT_GAMEACTION = "Action";
constexpr uint64_t RFCAT_GAMEACTION_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
