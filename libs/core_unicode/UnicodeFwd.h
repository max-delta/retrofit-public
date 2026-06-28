#pragma once

#include "rftl/cstdint"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

namespace Plane {
enum Value : uint8_t
{
	BasicMultilingual = 0,
	SupplementalMultilingual = 1,
	SupplementaryIdeographic = 2,
	TertiaryIdeographic = 3,
	SupplementaryPrivateUseAreaA = 15,
	SupplementaryPrivateUseAreaB = 16,

	NumPlanes = 17,
};
}

namespace PrivateUseArea {
enum Value : uint16_t
{
	Invalid = 0,

	// E000-F8FF
	BasicMultilingualPrivate_First = 0xE0,
	BasicMultilingualPrivate_Last = 0xF8,

	// F0000-FFFFF
	SupplementaryPrivateUseAreaA_First = 0xF00,
	SupplementaryPrivateUseAreaA_Last = 0xFFF,

	// 100000-10FFFD
	// NOTE: Last two characters are dubiously inconsistent, which seems like
	//  a standards bug
	SupplementaryPrivateUseAreaB_First = 0x1000,
	SupplementaryPrivateUseAreaB_Last = 0x10FF,
};
}

///////////////////////////////////////////////////////////////////////////////
}
