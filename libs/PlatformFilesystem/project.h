#pragma once
#include "Logging/Constants.h"

#ifdef PLATFORMFILESYSTEM_EXPORTS
#define PLATFORMFILESYSTEM_API __declspec( dllexport )
#else
#define PLATFORMFILESYSTEM_API __declspec( dllimport )
#endif

constexpr char const* RFCAT_VFS = "VFS";
constexpr uint64_t RFCAT_VFS_SEV_WHITELIST =
	RF::logging::kDefaultSeverityMask &
	~( RF::logging::RF_SEV_TRACE );
