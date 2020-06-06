#include "stdafx.h"
#include "ElementGrid.h"

#include "cc3o3/Common.h"
#include "cc3o3/char/CharFwd.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::ElementGrid )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::ElementGrid );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void ElementGrid::SetJustification( Justification justification )
{
	mJustification = justification;
}



void ElementGrid::UpdateFromCharacter( state::ObjectRef const& character )
{
	// TODO
	//RF_ASSERT( character.IsSet() );

	size_t const numTiles = mTileLayer.NumTiles();
	for( size_t i = 0; i < numTiles; i++ )
	{
		mTileLayer.GetMutableTile( i ).mIndex = 5;
	}
}



void ElementGrid::OnInstanceAssign( UIContext& context, Container& container )
{
	gfx::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kTilesetName );
	mTileLayer.ClearAndResize( character::kMaxElementLevels, character::kMaxSlotsPerElementLevel );
}



void ElementGrid::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::PPUCoord const expectedDimensions = {
		kTileWidth * character::kMaxElementLevels,
		kTileHeight * character::kMaxSlotsPerElementLevel
	};
	gfx::PPUCoord const pos = AlignToJustify( expectedDimensions, container.mAABB, mJustification );

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////
}
