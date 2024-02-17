#include "stdafx.h"

#include "CoffHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool CoffHeader::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesToRead = 20;
	rftl::array<uint8_t, kBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	mMachineType = math::enum_bitcast<MachineType>( math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() ) );
	mNumSections = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mTimeSinceUnixEpoch = rftl::chrono::seconds( math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() ) );
	mAbsoluteOffsetToSymbolTable = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mNumSymbols = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mOptionalHeaderBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mFlags = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	RF_ASSERT( readHead.empty() );
	mRelativeOffsetToOptionalHeader = kBytesToRead;
	mRelativeOffsetToFirstSectionHeader = mRelativeOffsetToOptionalHeader + mOptionalHeaderBytes;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
