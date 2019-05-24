#include "stdafx.h"
#include "DevTestCombatCharts.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestCombatCharts::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestCombatCharts::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
}



void DevTestCombatCharts::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestCombatCharts::OnTick( AppStateTickContext& context )
{
	//InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
