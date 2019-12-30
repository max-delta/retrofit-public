#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

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

		rollback::AutoVar<int16_t> mX;
		rollback::AutoVar<int16_t> mY;

		enum Facing : uint8_t
		{
			North = 0,
			East,
			South,
			West,
		};
		rollback::AutoVar<int16_t> mFacing;

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
