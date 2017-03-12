#pragma once
#include "core/ptr/ptr_base.h"
#include "core/ptr/creation_payload.h"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class UniquePtr : public PtrBase<T>
{
	//
	// Public methods
public:
	UniquePtr( CreationPayload<T> && payload )
		: PtrBase<T>(std::move(payload))
	{
		//
	}

	~UniquePtr()
	{
		DecreaseStrongCount();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
