#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API BitmapReader
{
	RF_NO_INSTANCE( BitmapReader );

public:
	// NOTE: Source is expected to be some kind of image file
	static Bitmap ReadRGBABitmap( void const* source, size_t len );
};

///////////////////////////////////////////////////////////////////////////////
}}
