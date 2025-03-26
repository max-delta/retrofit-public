#pragma once
#include "core/macros.h"

#include "rftl/extension/static_string.h"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

// Helper for attempting to get some basic information out of the processor
// NOTE: Realistically, this is only useful for x86, as ARM doesn't seem to
//  support this kind of in
class ProcessorInfo
{
	//
	// Public methods
public:
	ProcessorInfo() = default;

	static ProcessorInfo Generate();


	//
	// Public data
public:
	rftl::static_string<16> mVendorID;
	rftl::static_string<16> mModelID;
};

///////////////////////////////////////////////////////////////////////////////
}
