#include "stdafx.h"
#include "PaletteLoader.h"

#include "PPU/Palette.h"

#include "PlatformFilesystem/VFS.h"

#include "core_image/TempImageDecoder.h"
#include "core_math/Color4a5.h"
#include "core_vfs/FileBuffer.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

bool PaletteLoader::LoadPalette( Palette4a5_16& palette, file::VFS const& vfs, file::VFSPath const& path )
{
	palette = {};

	static constexpr size_t kExpectedPixels = rftl::remove_cvref<decltype( palette )>::type::kCount;
	static constexpr size_t kChannelsUsed = 3;

	// Open
	file::SeekHandlePtr const paletteHandle = vfs.GetFileForRead( path );
	if( paletteHandle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to open palette" );
		return false;
	}

	// Load
	file::FileBuffer const paletteBuffer{ *paletteHandle.Get(), false };
	if( paletteBuffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to load palette" );
		return false;
	}

	// Decode
	image::TempImageDecoder const decoded( paletteBuffer.GetBytes(), kChannelsUsed );
	rftl::byte_view const bytes = decoded.GetDecodedBytes();
	if( bytes.size() == 0 )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to decode palette" );
		return false;
	}

	// Validate
	size_t const numPixels = decoded.GetWidth() * decoded.GetHeight();
	if( numPixels != palette.size() )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Palette has {} pixels, expected {}", numPixels, kExpectedPixels );
		return false;
	}
	RF_ASSERT( bytes.size() == kExpectedPixels * kChannelsUsed );

	// Copy data
	rftl::byte_view readHead = bytes;
	for( size_t i = 0; i < kExpectedPixels; i++ )
	{
		math::Color4a5& dest = palette.at( i );

		uint8_t const r = readHead.front<uint8_t>();
		readHead.remove_prefix( 1 );
		uint8_t const g = readHead.front<uint8_t>();
		readHead.remove_prefix( 1 );
		uint8_t const b = readHead.front<uint8_t>();
		readHead.remove_prefix( 1 );

		dest = math::Color4a5( r, g, b );
	}
	RF_ASSERT( readHead.empty() );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
