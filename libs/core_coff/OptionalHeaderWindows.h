#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/iosfwd"
#include "rftl/optional"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct OptionalHeaderWindows
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase, OptionalHeaderType headerType, size_t remainingHeaderSize );

public:
	rftl::optional<size_t> mAbsoluteOffsetToData = rftl::nullopt;
	size_t mPreferredImageBase = 0;
	size_t mSectionMemoryAlignment = 0;
	size_t mSectionFileAlignment = 0;
	uint16_t mOSMajorVersion = 0;
	uint16_t mOSMinorVersion = 0;
	uint16_t mImageMajorVersion = 0;
	uint16_t mImageMinorVersion = 0;
	uint16_t mSubsystemMajorVersion = 0;
	uint16_t mSubsystemMinorVersion = 0;
	uint64_t mWin32Version = 0;
	uint64_t mImageMemoryBytes = 0;
	uint64_t mHeadersFileBytes = 0;
	uint64_t mChecksum = 0;
	uint16_t mSubsystem = 0;
	uint16_t mDLLFlags = 0;
	size_t mStackReserveBytes = 0;
	size_t mStackCommitBytes = 0;
	size_t mHeapReserveBytes = 0;
	size_t mHeapCommitBytes = 0;
	uint64_t mLoaderFlags = 0;
	size_t mNumDataDirectoryEntries = 0;
	size_t mRelativeOffsetToDataDirectory = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
