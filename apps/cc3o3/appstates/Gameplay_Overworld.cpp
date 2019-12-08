#include "stdafx.h"
#include "Gameplay_Overworld.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Overworld::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Overworld::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
}



void Gameplay_Overworld::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Overworld::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Overworld" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
