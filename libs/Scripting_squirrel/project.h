#pragma once
#include "Logging/Constants.h"

#ifdef SCRIPTINGSQUIRREL_EXPORTS
#define SCRIPTINGSQUIRREL_API __declspec(dllexport)
#else
#define SCRIPTINGSQUIRREL_API __declspec(dllimport)
#endif

constexpr char const* RFCAT_SQUIRREL = "Squirrel";
constexpr uint64_t RFCAT_SQUIRREL_SEV_WHITELIST = static_cast<uint64_t>( -1 );
