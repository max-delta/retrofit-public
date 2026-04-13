#pragma once
#include "Logging/Constants.h"

#ifndef RF_NO_SHARED_EXPORTS
#ifdef PLATFORMFILESYSTEM_EXPORTS
#define PLATFORMFILESYSTEM_API __declspec( dllexport )
#else
#define PLATFORMFILESYSTEM_API __declspec( dllimport )
#endif
#else
#define PLATFORMFILESYSTEM_API
#endif

inline constexpr char const RFCAT_VFS[] = "VFS";
inline constexpr uint64_t RFCAT_VFS_SEV_WHITELIST =
	RF::logging::kDefaultSeverityMask &
	~( RF::logging::RF_SEV_TRACE );
