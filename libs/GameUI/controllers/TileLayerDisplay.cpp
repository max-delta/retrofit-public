#include "stdafx.h"
#include "TileLayerDisplay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::TileLayerDisplay )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TileLayerDisplay );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void TileLayerDisplay::SetTileset( gfx::ManagedTilesetID tileset )
{
	mTileLayer.mTilesetReference = tileset;
}



gfx::TileLayer& TileLayerDisplay::GetMutableTileLayer()
{
	return mTileLayer;
}



void TileLayerDisplay::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::PPUCoord const pos = container.mAABB.mTopLeft;

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////
}
