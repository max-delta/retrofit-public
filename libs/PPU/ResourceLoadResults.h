#pragma once
#include "project.h"

#include "PPU/GfxFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/rf_assert.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

// Used by ResourceManager, primarily to reduce include weights when transiting
//  loads through interfaces and abstractions
template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
struct ResourceLoadResult
{
	//
	// Types and constants
public:
	static constexpr ManagedResourceID kInvalidResourceID = InvalidResourceID;


	//
	// Public methods
public:
	operator bool() const
	{
		if( mHandle == nullptr )
		{
			RF_ASSERT( mID == kInvalidResourceID );
			return false;
		}
		RF_ASSERT( mID != kInvalidResourceID );
		return true;
	}


	//
	// Public data
public:
	ManagedResourceID mID = kInvalidResourceID;
	WeakPtr<Resource const> mHandle = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
