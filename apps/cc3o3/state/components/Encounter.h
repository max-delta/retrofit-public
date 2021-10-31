#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/encounter/EncounterFwd.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"

#include "rftl/array"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Encounter final : public state::Component
{
	RF_NO_COPY( Encounter );

	//
	// Public methods
public:
	Encounter() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	// Whether or not a character is actively in the battle (even if dead)
	rftl::array<rollback::AutoVar<bool>, encounter::kMaxSpawns> mDeployed;
};

///////////////////////////////////////////////////////////////////////////////
}
