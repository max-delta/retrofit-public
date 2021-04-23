#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class OverworldMovement final : public state::Component
{
	RF_NO_COPY( OverworldMovement );

	//
	// Structs
public:
	struct Pos
	{
		RF_NO_COPY( Pos );
		Pos() = default;

		void Bind( rollback::Window& window, state::VariableIdentifier const& parent, alloc::Allocator& allocator );

		phys::PhysCoord GetCoord() const;
		void SetCoord( phys::PhysCoord pos );

		rollback::AutoVar<phys::PhysCoordElem> mX;
		rollback::AutoVar<phys::PhysCoordElem> mY;

		rollback::AutoVar<phys::Direction::Value> mFacing;

		rollback::AutoVar<bool> mMoving;
	};


	//
	// Public methods
public:
	OverworldMovement() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<1024>> mAlloc{ ExplicitDefaultConstruct() };

	Pos mCurPos = {};
};

///////////////////////////////////////////////////////////////////////////////
}
