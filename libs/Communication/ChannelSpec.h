#pragma once
#include "project.h"

#include "Communication/CommunicationFwd.h"

#include "rftl/vector"


namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

struct COMMUNICATION_API ChannelSpec
{
	//
	// Types and constants
public:
	enum class Viability : uint8_t
	{
		Preferred = 0,
		Fallback = 1,
		// Nonviable = <Not listed>
	};
	using Flag = rftl::pair<ChannelFlags::Value, Viability>;
	using Flags = rftl::vector<Flag>;


	//
	// Public data
public:
	Flags mFlags;
};

///////////////////////////////////////////////////////////////////////////////
}
