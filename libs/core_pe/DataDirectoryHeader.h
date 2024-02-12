#pragma once
#include "core_pe/PeFwd.h"

#include "rftl/iosfwd"


namespace RF::bin::pe {
///////////////////////////////////////////////////////////////////////////////

struct DataDirectoryHeader
{
public:
	struct Entry
	{
		size_t mAbsoluteOffsetToData = 0;
		size_t mDataBytes = 0;
	};

public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase, size_t numEntries, size_t remainingHeaderSize );

public:
	Entry mExportTable = {};
	Entry mImportTable = {};
	Entry mResourceTable = {};
	Entry mExceptionTable = {};
	Entry mCertificateTable = {};
	Entry mBaseRelocationTable = {};
	Entry mDebug = {};
	Entry mArchitecture = {};
	Entry mGlobalPointer = {};
	Entry mTlsTable = {};
	Entry mLoadConfigTable = {};
	Entry mBoundImportTable = {};
	Entry mImportAddressTable = {};
	Entry mDelayImportTable = {};
	Entry mClrHeader = {};
};

///////////////////////////////////////////////////////////////////////////////
}
