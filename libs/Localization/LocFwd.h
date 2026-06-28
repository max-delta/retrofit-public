#pragma once
#include "core_localization/LocFwd.h"
#include "core_unicode/UnicodeFwd.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

enum class TextDirection : uint8_t
{
	kInvalid = 0,
	LeftToRight,
	RightToLeft
};

// Code points in this private use area are treated specially by the
//  page mapper, where the lowest byte of the codepoint is extracted and output
//  directly during page mapping, effectively bypassing the mapping so that
//  control codes for terminals have a mechanism to pass through unmodified
inline constexpr unicode::PrivateUseArea::Value kPageMapperCollapseArea{ 0xE0u };

class LocKey;
class LocQuery;
class LocResult;
class LocEngine;
class PageMapper;

///////////////////////////////////////////////////////////////////////////////
}
