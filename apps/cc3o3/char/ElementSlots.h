#pragma once
#include "project.h"

#include "rftl/string"
#include "rftl/array"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct ElementSlots
{
	static constexpr size_t kMaxLevels = 8;
	static constexpr size_t kMaxSlots = 8;

	using Slot = rftl::string;
	using Grid = rftl::array<Slot, kMaxLevels * kMaxSlots>;

	Grid mGrid = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}
