#include "stdafx.h"

#include "DosHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

bool DosHeader::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	rftl::array<uint8_t, 64> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );

	if( bufferView.substr( 0, 2 ) != rftl::byte_view( "MZ", 2 ) )
	{
		// Invalid magic
		return false;
	}

	// Offset to PE header
	mAbsoluteOffsetToPEHeader = math::FromLittleEndianToPlatform( bufferView.at<uint32_t>( 60 ) );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
