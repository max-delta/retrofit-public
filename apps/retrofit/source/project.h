#pragma once
#include "Logging/Constants.h"

constexpr char const* RFCAT_STARTUP = "Startup";
constexpr uint64_t RFCAT_STARTUP_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;

constexpr char const* RFCAT_STARTUPTEST = "StartupTest";
constexpr uint64_t RFCAT_STARTUPTEST_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;

constexpr char const* RFCAT_FRAMEPACKEDITOR = "FramePackEditor";
constexpr uint64_t RFCAT_FRAMEPACKEDITOR_SEV_WHITELIST = RF::logging::kDefaultSeverityMask;
