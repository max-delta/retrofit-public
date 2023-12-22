#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"

#include "rftl/array"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

// Mask out elements in a grid granularly using bits
struct GridBitMask
{
	//
	// Types and constants
private:
	static_assert( kMaxSlotsPerElementLevel == 8 );
	using ColumnBits = uint8_t;
	static constexpr size_t kBitsPerColumn = 8;
	using ColumnsByElemLevel = rftl::array<ColumnBits, element::kNumElementLevels>;


	//
	// Public methods
public:
	// NOTE: Grid bits default to false
	GridBitMask() = default;
	explicit GridBitMask( bool initialValue );

	// Get the bit value
	// NOTE: Out-of-bounds bits will assert and act as though they are false
	bool FetchBit( character::ElementSlotIndex slotIndex ) const;

	// Sets the bit value, returns the previous value
	// NOTE: Out-of-bounds bits will assert and act as though they are false
	bool AssignBit( character::ElementSlotIndex slotIndex, bool newValue );

	// Assigns all bits to the specified value
	void AssignAllBits( bool newValue );


	//
	// Private data
private:
	ColumnsByElemLevel mColumnsByElemLevel = {};
};

///////////////////////////////////////////////////////////////////////////////
}
