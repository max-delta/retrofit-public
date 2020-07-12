#include "stdafx.h"
#include "Gameplay_Battle.h"

#include "cc3o3/Common.h"
#include "cc3o3/combat/CombatInstance.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Battle::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	UniquePtr<combat::CombatInstance> mCombatInstance;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Battle::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	// Setup combat instance
	{
		mInternalState->mCombatInstance = DefaultCreator<combat::CombatInstance>::Create( gCombatEngine );

		// TODO: Add combatants
	}
}



void Gameplay_Battle::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Battle::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Battle" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
