#include "stdafx.h"
#include "BitmapWriter.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

rftl::vector<uint8_t> RF::sprite::BitmapWriter::WriteRGBABitmap( math::Color4u8 const* source, size_t width, size_t height )
{
	// NOTE: Rows are 32-bit aligned, but as long as we write 32-bit pixels, we
	//  won't need to add any padding
	static_assert( sizeof( uint8_t ) * 4 == sizeof( uint32_t ), "Unexpected size" );

	// NOTE: Using the lowest version that officialy supported 32-bit pixels to
	//  be directly written into the bitmap data without palletes
	constexpr size_t kFileHeaderSize = 14;
	constexpr size_t kBitmapHeaderSize = 40;
	constexpr size_t kBytesPerPixel = 4;
	size_t const numPixels = width * height;
	size_t const bitmapDataSize = numPixels * kBytesPerPixel;
	size_t const totalFileSize = kFileHeaderSize + kBitmapHeaderSize + bitmapDataSize;

	rftl::vector<uint8_t> retVal;
	retVal.resize( totalFileSize );

	void* writeHead = retVal.data();
	static constexpr auto write8 = []( void*& writeHead, uint8_t const& data )
	{
		uint8_t*& castHead = reinterpret_cast<uint8_t*&>( writeHead );
		*castHead = math::FromPlatformToLittleEndian( data );
		castHead++;
	};
	static constexpr auto write16 = []( void*& writeHead, uint16_t const& data )
	{
		uint16_t*& castHead = reinterpret_cast<uint16_t*&>( writeHead );
		*castHead = math::FromPlatformToLittleEndian( data );
		castHead++;
	};
	static constexpr auto write32 = []( void*& writeHead, uint32_t const& data )
	{
		uint32_t*& castHead = reinterpret_cast<uint32_t*&>( writeHead );
		*castHead = math::FromPlatformToLittleEndian( data );
		castHead++;
	};

	// File header
	{
		constexpr char kSigB = 'B';
		constexpr char kSigM = 'M';
		uint32_t const fileSize = math::integer_cast<uint32_t>( totalFileSize );
		constexpr uint16_t kReserved1 = 0;
		constexpr uint16_t kReserved2 = 0;
		constexpr uint32_t kFileOffsetToPixelArray = kFileHeaderSize + kBitmapHeaderSize;

		write8( writeHead, kSigB );
		write8( writeHead, kSigM );
		write32( writeHead, fileSize );
		write16( writeHead, kReserved1 );
		write16( writeHead, kReserved2 );
		write32( writeHead, kFileOffsetToPixelArray );
		RF_ASSERT( writeHead == retVal.data() + kFileHeaderSize );
	}

	// Bitmap header
	{
		constexpr uint32_t headerSize = kBitmapHeaderSize;
		uint32_t const bitmapWidth = math::integer_cast<uint32_t>( width ); // Left-to-right
		uint32_t const bitmapHeight = static_cast<uint32_t>( -math::integer_cast<int32_t>( height ) ); // Top-to-bottom
		constexpr uint16_t kColorPlanes = 1;
		constexpr uint16_t kBitsPerPixel = kBytesPerPixel * 8;
		constexpr uint32_t kCompressionType = 0; // Uncompressed
		constexpr uint32_t kBitmapSizeInHeader = 0; // Uncompressed
		constexpr uint32_t kHoriResolutionPixelsPerMeter = 1000; // Millimeter
		constexpr uint32_t kVertResolutionPixelsPerMeter = 1000; // Millimeter
		constexpr uint32_t kNumColorsInPallete = 0; // No pallete
		constexpr uint32_t kNumImportantColors = 0; // All colors meaningful

		write32( writeHead, headerSize );
		write32( writeHead, bitmapWidth );
		write32( writeHead, bitmapHeight );
		write16( writeHead, kColorPlanes );
		write16( writeHead, kBitsPerPixel );
		write32( writeHead, kCompressionType );
		write32( writeHead, kBitmapSizeInHeader );
		write32( writeHead, kHoriResolutionPixelsPerMeter );
		write32( writeHead, kVertResolutionPixelsPerMeter );
		write32( writeHead, kNumColorsInPallete );
		write32( writeHead, kNumImportantColors );
		RF_ASSERT( writeHead == retVal.data() + kFileHeaderSize + kBitmapHeaderSize );
	}

	// Bitmap data
	for( size_t i = 0; i < numPixels; i++ )
	{
		math::Color4u8 const& pixel = source[i];
		write8( writeHead, pixel.b );
		write8( writeHead, pixel.g );
		write8( writeHead, pixel.r );
		write8( writeHead, pixel.a );
		RF_ASSERT( writeHead <= retVal.data() + kFileHeaderSize + kBitmapHeaderSize + bitmapDataSize );
	}
	RF_ASSERT( writeHead == retVal.data() + kFileHeaderSize + kBitmapHeaderSize + bitmapDataSize );

	RF_ASSERT( writeHead == retVal.data() + totalFileSize );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
