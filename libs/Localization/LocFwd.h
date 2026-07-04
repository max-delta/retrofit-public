#pragma once
#include "core_localization/LocFwd.h"
#include "core_unicode/UnicodeFwd.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

// Code points in this private use area are treated specially by the
//  page mapper, where the lowest byte of the codepoint is extracted and output
//  directly during page mapping, effectively bypassing the mapping so that
//  control codes for terminals have a mechanism to pass through unmodified
inline constexpr unicode::PrivateUseArea::Value kPageMapperCollapseArea{ 0xE0u };

// Localization keys are in the form "$key_name"
inline constexpr char kKeyPrefix = U'$';

// Replacement tokens are in the form "{$key_name}"
inline constexpr char32_t const kReplacementTokenOpen[] = U"{$";
inline constexpr char32_t const kReplacementTokenClose[] = U"}";

class LocKey;
class LocQuery;
class LocResult;
class LocEngine;
class PageMapper;

///////////////////////////////////////////////////////////////////////////////
}
