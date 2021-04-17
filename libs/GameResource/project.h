#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef GAMERESOURCE_EXPORTS
		#define GAMERESOURCE_API __declspec( dllexport )
	#else
		#define GAMERESOURCE_API __declspec( dllimport )
	#endif
#else
	#define GAMERESOURCE_API
#endif

constexpr char const* RFCAT_GAMERESOURCE = "Resource";
constexpr uint64_t RFCAT_GAMERESOURCE_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
