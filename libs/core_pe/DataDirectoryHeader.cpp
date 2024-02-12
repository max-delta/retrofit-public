#include "stdafx.h"

#include "DataDirectoryHeader.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "rftl/extension/byte_view.h"
#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

bool DataDirectoryHeader::TryRead( rftl::streambuf& seekable, size_t seekBase, size_t numEntries, size_t remainingHeaderSize )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesPerEntry = 8;
	size_t const theoreticalBytesToRead = numEntries * kBytesPerEntry;
	if( remainingHeaderSize < theoreticalBytesToRead )
	{
		// Not enough bytes
		return false;
	}

	static constexpr size_t kMaxKnownEntries = 15;
	static constexpr size_t kMaxBytesToRead = kMaxKnownEntries * kBytesPerEntry;
	size_t const actualBytesToRead = math::Min(
		kMaxBytesToRead,
		theoreticalBytesToRead );

	rftl::array<uint8_t, kMaxBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), actualBytesToRead, stream );
	if( numRead != actualBytesToRead )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.begin() + math::integer_cast<ptrdiff_t>( actualBytesToRead ) );
	rftl::byte_view readHead = bufferView;

	static constexpr auto tryRead = []( rftl::byte_view& readHead, Entry& entry ) -> bool
	{
		if( readHead.empty() )
		{
			return false;
		}
		entry.mAbsoluteOffsetToData = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
		entry.mDataBytes = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
		return true;
	};

	tryRead( readHead, mExportTable );
	tryRead( readHead, mImportTable );
	tryRead( readHead, mResourceTable );
	tryRead( readHead, mExceptionTable );
	tryRead( readHead, mCertificateTable );
	tryRead( readHead, mBaseRelocationTable );
	tryRead( readHead, mDebug );
	tryRead( readHead, mArchitecture );
	tryRead( readHead, mGlobalPointer );
	tryRead( readHead, mTlsTable );
	tryRead( readHead, mLoadConfigTable );
	tryRead( readHead, mBoundImportTable );
	tryRead( readHead, mImportAddressTable );
	tryRead( readHead, mDelayImportTable );
	tryRead( readHead, mClrHeader );
	RF_ASSERT( readHead.empty() );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
