#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "rftl/extension/byte_view.h"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

// NOTE: It is undesirable, but technically correct to throw away the alpha, as
//  the standard says to just ignore those bytes. RetroFit's reader will not
//  discard this information at time of writing, and this is intentional.
// SEE: Windows Metafile Format v20180912 2.1.1.3
// SEE: BitmapReader
class GAMESPRITE_API BitmapReader
{
	RF_NO_INSTANCE( BitmapReader );

public:
	// NOTE: Source is expected to be some kind of image file
	static Bitmap ReadRGBABitmap( rftl::byte_view const& buffer );
};

///////////////////////////////////////////////////////////////////////////////
}
