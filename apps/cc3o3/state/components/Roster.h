#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"
#include "cc3o3/company/CompanyFwd.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"

#include "rftl/array"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class Roster final : public state::Component
{
	RF_NO_COPY( Roster );

	//
	// Public methods
public:
	Roster() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	// Whether or not a character is eligible (as dictated by story)
	rftl::array<rollback::AutoVar<bool>, company::kRosterSize> mEligible;

	// The active team members, a subset of the roster
	// NOTE: Some slots may be invalid
	rftl::array<rollback::AutoVar<company::RosterIndex>, company::kActiveTeamSize> mActiveTeam;
};

///////////////////////////////////////////////////////////////////////////////
}
