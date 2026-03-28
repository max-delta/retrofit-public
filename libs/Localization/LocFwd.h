#pragma once
#include "core_localization/LocFwd.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

enum class TextDirection : uint8_t
{
	kInvalid = 0,
	LeftToRight,
	RightToLeft
};

class LocKey;
class LocQuery;
class LocResult;
class LocEngine;
class PageMapper;

///////////////////////////////////////////////////////////////////////////////
}
