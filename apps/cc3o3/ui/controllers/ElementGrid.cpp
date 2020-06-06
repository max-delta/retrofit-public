#include "stdafx.h"
#include "ElementGrid.h"

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
	mCache.UpdateFromCharacter( character, false );

	ElementGridDisplayCache::Grid const& grid = mCache.GetGridRef();
	for( size_t i_col = 0; i_col < grid.size(); i_col++ )
	{
		ElementGridDisplayCache::Column const& column = grid.at( i_col );
		for( size_t i_row = 0; i_row < column.size(); i_row++ )
		{
			ElementGridDisplayCache::Slot const& slot = column.at( i_row );
			mTileLayer.GetMutableTile( i_col, i_row ).mIndex = math::enum_bitcast( slot.mTilesetIndex );
		}
	}
}



void ElementGrid::UpdateFromCache( ElementGridDisplayCache const& cache )
{
	mCache = cache;
}



void ElementGrid::OnInstanceAssign( UIContext& context, Container& container )
{
	gfx::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kElementTilesetMiniName );
	mTileLayer.ClearAndResize( character::kMaxElementLevels, character::kMaxSlotsPerElementLevel );
}



void ElementGrid::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::PPUCoord const expectedDimensions = {
		kElementTileMiniWidth * character::kMaxElementLevels,
		kElementTileMiniHeight * character::kMaxSlotsPerElementLevel
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
