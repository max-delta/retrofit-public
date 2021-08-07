#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/combat/CombatantID.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Combo final : public state::Component
{
	RF_NO_COPY( Combo );

	//
	// Public methods
public:
	Combo() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<512>> mAlloc{ ExplicitDefaultConstruct() };

	rollback::AutoVar<combat::SimVal> mComboMeter;
	rollback::AutoVar<combat::FighterID::Value> mComboTarget;
};

///////////////////////////////////////////////////////////////////////////////
}
