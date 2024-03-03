#pragma once
#include "core_coff/CoffFwd.h"

#include "rftl/extension/static_string.h"
#include "rftl/iosfwd"
#include "rftl/optional"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

struct SymbolRecord
{
public:
	bool TryRead( rftl::streambuf& seekable, size_t seekBase );

public:
	rftl::static_string<8> mName = {};
	uint32_t mValue = 0;
	size_t mSectionNumber = 0;
	SymbolTypeUpper mTypeUpper = {};
	SymbolTypeLower mTypeLower = {};
	SymbolStorageClass mStorageClass = {};
	size_t mNumAuxSymbols = 0;
	size_t mRelativeOffsetToFirstAuxSymbol = 0;
	size_t mRelativeOffsetToNextSymbol = 0;
	rftl::optional<size_t> mNameOffsetIntoStringTable = rftl::nullopt;
};

///////////////////////////////////////////////////////////////////////////////
}
