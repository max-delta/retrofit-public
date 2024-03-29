#include "stdafx.h"

#include "OptionalHeaderWindows.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool OptionalHeaderWindows::TryRead( rftl::streambuf& seekable, size_t seekBase, OptionalHeaderType headerType, size_t remainingHeaderSize )
{
	*this = {};

	bool const isPeHeader =
		headerType == OptionalHeaderType::PE32 ||
		headerType == OptionalHeaderType::PE32Plus;
	if( isPeHeader == false )
	{
		return false;
	}
	bool const isPlusHeader = headerType == OptionalHeaderType::PE32Plus;

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	// 44 for PE32, 88 for PE32+
	static constexpr size_t kMaxBytesToRead = 88ull;
	size_t const bytesToRead = isPlusHeader ? kMaxBytesToRead : 44ull;
	if( remainingHeaderSize < bytesToRead )
	{
		// Not enough bytes
		return false;
	}

	rftl::array<uint8_t, kMaxBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), bytesToRead, stream );
	if( numRead != bytesToRead )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.begin() + math::integer_cast<ptrdiff_t>( bytesToRead ) );
	rftl::byte_view readHead = bufferView;

	if( isPlusHeader == false )
	{
		mAbsoluteOffsetToData = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	}

	mPreferredImageBase = math::integer_cast<size_t>( math::FromLittleEndianToPlatform(
		isPlusHeader ? readHead.extract_front<uint64_t>() : readHead.extract_front<uint32_t>() ) );
	mSectionMemoryAlignment = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mSectionFileAlignment = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mOSMajorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mOSMinorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mImageMajorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mImageMinorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mSubsystemMajorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mSubsystemMinorVersion = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mWin32Version = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mImageMemoryBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mHeadersFileBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mChecksum = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mSubsystem = math::enum_bitcast<WindowsSubsystemType>( math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() ) );
	mDLLFlags = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mStackReserveBytes = math::integer_cast<size_t>( math::FromLittleEndianToPlatform(
		isPlusHeader ? readHead.extract_front<uint64_t>() : readHead.extract_front<uint32_t>() ) );
	mStackCommitBytes = math::integer_cast<size_t>( math::FromLittleEndianToPlatform(
		isPlusHeader ? readHead.extract_front<uint64_t>() : readHead.extract_front<uint32_t>() ) );
	mHeapReserveBytes = math::integer_cast<size_t>( math::FromLittleEndianToPlatform(
		isPlusHeader ? readHead.extract_front<uint64_t>() : readHead.extract_front<uint32_t>() ) );
	mHeapCommitBytes = math::integer_cast<size_t>( math::FromLittleEndianToPlatform(
		isPlusHeader ? readHead.extract_front<uint64_t>() : readHead.extract_front<uint32_t>() ) );
	mLoaderFlags = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mNumDataDirectoryEntries = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	RF_ASSERT( readHead.empty() );
	mRelativeOffsetToDataDirectory = bytesToRead;
	RF_ASSERT( mRelativeOffsetToDataDirectory <= remainingHeaderSize );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
