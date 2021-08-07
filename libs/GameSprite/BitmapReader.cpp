#include "stdafx.h"
#include "BitmapReader.h"

#include "GameSprite/Bitmap.h"

#include "core_math/math_casts.h"

#include "stb_image/stb_image.h"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

Bitmap BitmapReader::ReadRGBABitmap( rftl::byte_view const& buffer )
{
	int x, y, n;
	unsigned char* data = stbi_load_from_memory( reinterpret_cast<stbi_uc const*>( buffer.data() ), math::integer_cast<int>( buffer.size() ), &x, &y, &n, 4 );
	RF_ASSERT( data != nullptr );
	if( data == nullptr )
	{
		return Bitmap( ExplicitDefaultConstruct() );
	}

	// Create bitmap to store
	size_t const width = math::integer_cast<size_t>( x );
	size_t const height = math::integer_cast<size_t>( y );
	Bitmap retVal( width, height );

	// Copy data
	uint8_t const* readHead = data;
	for( size_t scanline = 0; scanline < height; scanline++ )
	{
		for( size_t pixel = 0; pixel < width; pixel++ )
		{
			Bitmap::ElementType& dest = retVal.GetMutablePixel( pixel, scanline );
			dest.r = *readHead;
			readHead++;
			dest.g = *readHead;
			readHead++;
			dest.b = *readHead;
			readHead++;
			dest.a = *readHead;
			readHead++;
		}
	}

	// Free resource
	stbi_image_free( data );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
