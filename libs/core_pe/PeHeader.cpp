#include "stdafx.h"

#include "PeHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

bool PeHeader::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	rftl::array<uint8_t, 4> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );

	if( bufferView.substr( 0, 4 ) != rftl::byte_view( "PE\0\0", 4 ) )
	{
		// Invalid magic
		return false;
	}

	// Offset to COFF header
	// NOTE: COFF header is missing magic, which is normally 2 bytes, and the
	//  PE magic is 4 bytes, so we're offsetting by the difference and will
	//  need to bypass the magic check on the COFF header
	mRelativeOffsetToCOFFHeader = 2;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
