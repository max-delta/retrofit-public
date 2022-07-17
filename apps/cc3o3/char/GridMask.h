#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"
#include "cc3o3/company/CompanyFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/array"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct GridMask
{
	//
	// Types and constants
public:
	using SlotsPerElemLevel = rftl::array<size_t, element::kNumElementLevels>;


	//
	// Public methods
public:
	GridMask() = default;

	size_t const& GetNumSlotsAtLevel( element::ElementLevel level ) const;

	static size_t CalcMinimumTotalSlots( company::StoryTier storyTier );
	static GridMask CalcMinimumSlots( company::StoryTier storyTier );

	static size_t CalcTotalSlots( StatValue elemPower, company::StoryTier storyTier );
	static GridMask CalcSlots( StatValue elemPower, GridShape gridShape, company::StoryTier storyTier );


	//
	// Public data
public:
	SlotsPerElemLevel mSlotsPerElemLevel = {};
};

///////////////////////////////////////////////////////////////////////////////
}
