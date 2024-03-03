#include "stdafx.h"

#include "StringTableHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool StringTableHeader::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesToRead = 4;
	rftl::array<uint8_t, kBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	mSize = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	RF_ASSERT( readHead.empty() );

	return true;
}



rftl::vector<uint8_t> StringTableHeader::TryCopyBytes( rftl::streambuf& seekable, size_t seekBase ) const
{
	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return {};
	}

	rftl::vector<uint8_t> buffer = {};
	buffer.resize( mSize );
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return {};
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	// Sanity check, in case the data became corrupted since the last call, or
	//  the caller gave a bad seek base that differed from the header read
	size_t const size = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	if( size != mSize )
	{
		RF_DBGFAIL();
		return {};
	}

	return buffer;
}

///////////////////////////////////////////////////////////////////////////////
}
