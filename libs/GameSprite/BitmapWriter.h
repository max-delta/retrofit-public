#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "core_math/Color4u8.h"

#include "rftl/vector"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API BitmapWriter
{
	RF_NO_INSTANCE( BitmapWriter );

public:
	// NOTE: Source is expected to be left-to-right scanlines in top-to-bottom
	//  order as a contiguous buffer
	static rftl::vector<uint8_t> WriteRGBABitmap( math::Color4u8 const* source, size_t rows, size_t columns );
};

///////////////////////////////////////////////////////////////////////////////
}}
