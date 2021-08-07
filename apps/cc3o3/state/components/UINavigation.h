#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "Rollback/AutoVar.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/Allocator.h"

#include "core/macros.h"

#include "rftl/array"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class UINavigation final : public state::Component
{
	RF_NO_COPY( UINavigation );


	//
	// Public methods
public:
	UINavigation() = default;

	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override;


	//
	// Public data
public:
	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	struct PauseMenu
	{
		RF_NO_COPY( PauseMenu );
		PauseMenu() = default;

		rollback::AutoVar<uint8_t> mNavigationDepth;
		rollback::AutoVar<uint8_t> mTopLevelCursor;
	} mPauseMenu;

	struct BattleMenu
	{
		RF_NO_COPY( BattleMenu );
		BattleMenu() = default;

		rollback::AutoVar<uint8_t> mControlCharIndex;
		rollback::AutoVar<uint8_t> mControlState;
		rftl::array<rollback::AutoVar<uint8_t>, 2> mCursorIndex;
	} mBattleMenu;
};

///////////////////////////////////////////////////////////////////////////////
}
