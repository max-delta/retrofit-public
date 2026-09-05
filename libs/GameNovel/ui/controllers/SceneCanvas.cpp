#include "stdafx.h"
#include "SceneCanvas.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::novel::ui::controller::SceneCanvas )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::novel::ui::controller::SceneCanvas );
}

namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////

SceneCanvas::SceneCanvas() = default;



void SceneCanvas::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	( (void)container.mAABB );
	gfx::ppu::DepthLayer const zLayer =
		context.GetContainerManager().GetRecommendedRenderDepth( container );
	RF_TODO_ANNOTATION(
		"Callback to novel to hand over AABB for it to render to? Implement inline?" );

	if( mTODOTileLayer.NumTiles() == 0 )
	{
		return;
	}

	mTODOTileLayer.mXCoord = 0;
	mTODOTileLayer.mYCoord = 0;
	mTODOTileLayer.mZLayer = zLayer;

	renderer.DrawTileLayer( mTODOTileLayer );
}

void SceneCanvas::ClearAll()
{
	mTODOTileLayer = {};
}

///////////////////////////////////////////////////////////////////////////////
}
