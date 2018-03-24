#pragma once
#include "Logging/Constants.h"

#ifdef SERIALIZATION_EXPORTS
#define SERIALIZATION_API __declspec(dllexport)
#else
#define SERIALIZATION_API __declspec(dllimport)
#endif

constexpr char const* RFCAT_SERIALIZATION = "Serialization";
constexpr uint64_t RFCAT_SERIALIZATION_SEV_WHITELIST = static_cast<uint64_t>( -1 );
