#pragma once
#include "Logging/Constants.h"

#ifdef APPCOMMONGRAPHICALCLIENT_EXPORTS
#define APPCOMMONGRAPHICALCLIENT_API __declspec(dllexport)
#else
#define APPCOMMONGRAPHICALCLIENT_API __declspec(dllimport)
#endif

constexpr char const* RFCAT_STARTUP = "Startup";
constexpr uint64_t RFCAT_STARTUP_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
