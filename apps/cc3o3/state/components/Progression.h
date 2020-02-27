#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Progression final : public state::Component
{
	RF_NO_COPY( Progression );


	//
	// Public methods
public:
	Progression() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	// Major story progression
	//  * 0: Invalid
	//  * 1-3: Tutorial
	//  * 4-8: Element level unlocks
	//  * 9+: Unbalanced
	rollback::AutoVar<uint8_t> mStoryTier;
};

///////////////////////////////////////////////////////////////////////////////
}
