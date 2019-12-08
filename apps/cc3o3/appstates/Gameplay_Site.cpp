#include "stdafx.h"
#include "Gameplay_Site.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Site::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Site::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
}



void Gameplay_Site::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Site::OnTick( AppStateTickContext& context )
{
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Site" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
