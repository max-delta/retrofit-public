#include "stdafx.h"
#include "BitmapReader.h"

#include "GameSprite/Bitmap.h"

#include "core_image/TempImageDecoder.h"
#include "core_math/math_casts.h"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

Bitmap BitmapReader::ReadRGBABitmap( rftl::byte_view const& buffer )
{
	image::TempImageDecoder const decoded( buffer, 4 );
	rftl::byte_view const bytes = decoded.GetDecodedBytes();
	if( bytes.size() == 0 )
	{
		return Bitmap( ExplicitDefaultConstruct() );
	}

	// Create bitmap to store
	size_t const width = decoded.GetWidth();
	size_t const height = decoded.GetHeight();
	Bitmap retVal( width, height );

	// Copy data
	rftl::byte_view readHead = bytes;
	for( size_t scanline = 0; scanline < height; scanline++ )
	{
		for( size_t pixel = 0; pixel < width; pixel++ )
		{
			Bitmap::ElementType& dest = retVal.GetMutablePixel( pixel, scanline );
			dest.r = readHead.front<uint8_t>();
			readHead.remove_prefix( 1 );
			dest.g = readHead.front<uint8_t>();
			readHead.remove_prefix( 1 );
			dest.b = readHead.front<uint8_t>();
			readHead.remove_prefix( 1 );
			dest.a = readHead.front<uint8_t>();
			readHead.remove_prefix( 1 );
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
