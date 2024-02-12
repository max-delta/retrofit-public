#include "stdafx.h"

#include "OptionalHeaderCommon.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool OptionalHeaderCommon::TryRead( rftl::streambuf& seekable, size_t seekBase, size_t remainingHeaderSize )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesToRead = 24;
	if( remainingHeaderSize < kBytesToRead )
	{
		// Not enough bytes
		return false;
	}

	rftl::array<uint8_t, kBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	mHeaderType = math::enum_bitcast<OptionalHeaderType>( math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() ) );
	mVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mCodeBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mInitializedBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mUninitializedBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToEntryPoint = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToCode = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	RF_ASSERT( readHead.empty() );
	mRelativeOffsetToPlatformHeader = kBytesToRead;
	RF_ASSERT( mRelativeOffsetToPlatformHeader <= remainingHeaderSize );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
