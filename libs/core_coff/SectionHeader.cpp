#include "stdafx.h"

#include "SectionHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool SectionHeader::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesToRead = 40;
	rftl::array<uint8_t, kBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	static constexpr size_t kNameLen = 8;
	static_assert( decltype( mName )::fixed_capacity == kNameLen );
	mName.resize( kNameLen, '?' );
	readHead.mem_copy_prefix_to( mName );
	readHead.remove_prefix( kNameLen );

	mMemoryBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToMemoryAddress = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mRawDataFileBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToAddress = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToRelocations = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mAbsoluteOffsetToLineNumbers = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mNumRelocations = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mNumLineNumbers = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mFlags = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	RF_ASSERT( readHead.empty() );
	mRelativeOffsetToNextSection = kBytesToRead;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
