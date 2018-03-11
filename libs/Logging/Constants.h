#pragma once
#include "project.h"

#include <stdint.h>


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

enum Severity : uint64_t
{
	RF_SEV_TRACE = 1 << 0,
	RF_SEV_DEBUG = 1 << 1,
	RF_SEV_INFO = 1 << 2,
	RF_SEV_WARNING = 1 << 3,
	RF_SEV_ERROR = 1 << 4,
	RF_SEV_CRITICAL = 1 << 5,
	___RF_SEV_RESERVED_6 = 1 << 6,
	___RF_SEV_RESERVED_7 = 1 << 7,

	// Bits 9-56 (shifts 8-55) are for custom user severities

	___RF_SEV_RESERVED_56 = 1ull << 56,
	___RF_SEV_RESERVED_57 = 1ull << 57,
	___RF_SEV_RESERVED_58 = 1ull << 58,
	___RF_SEV_RESERVED_59 = 1ull << 59,
	___RF_SEV_RESERVED_60 = 1ull << 60,
	___RF_SEV_RESERVED_61 = 1ull << 61,
	RF_SEV_CALLSTACK_VALUABLE = 1ull << 62,
	RF_SEV_UNRECOVERABLE = 1ull << 63
};

// This macro can be defined to globally restrict what types of severities can
//  be whitelisted for categories
#ifndef ___RFLOG_GLOBAL_SEV_WHITELIST_MASK
	#define ___RFLOG_GLOBAL_SEV_WHITELIST_MASK -1
#endif

///////////////////////////////////////////////////////////////////////////////
}}
