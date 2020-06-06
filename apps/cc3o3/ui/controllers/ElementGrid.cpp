#include "stdafx.h"
#include "ElementGrid.h"

#include "cc3o3/Common.h"
#include "cc3o3/char/CharFwd.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"
#include "core_math/Lerp.h"

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

	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - expectedDimensions.y;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top();
		gfx::PPUCoordElem const bottom = container.mAABB.Bottom() - expectedDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = container.mAABB.Left();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Right ) )
	{
		pos.x = container.mAABB.Right() - expectedDimensions.x;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::PPUCoordElem const left = container.mAABB.Left();
		gfx::PPUCoordElem const right = container.mAABB.Right() - expectedDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////
}
