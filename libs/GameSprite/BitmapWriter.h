#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "core_math/Color4u8.h"

#include "rftl/vector"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

// NOTE: There is an extremely high likelihood that your image viewing program
//  will throw away the alpha channel. This is undesirable, but technically
//  correct, as the standard says to just ignore those bytes. RetroFit's reader
//  will not discard this information at time of writing, and this is
//  intentional.
// SEE: Windows Metafile Format v20180912 2.1.1.3
// SEE: BitmapReader
class GAMESPRITE_API BitmapWriter
{
	RF_NO_INSTANCE( BitmapWriter );

public:
	// NOTE: Source is expected to be left-to-right scanlines in top-to-bottom
	//  order as a contiguous buffer
	static rftl::vector<uint8_t> WriteRGBABitmap( math::Color4u8 const* source, size_t width, size_t height );
};

///////////////////////////////////////////////////////////////////////////////
}
