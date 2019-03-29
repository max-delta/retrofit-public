#pragma once
#include "rftl/cstdint"


namespace RF { namespace loc {
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
}}
