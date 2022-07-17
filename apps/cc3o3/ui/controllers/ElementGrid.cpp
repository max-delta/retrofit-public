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

ElementGrid::ElementGrid( ElementTileSize size )
	: mSize( size )
{
	//
}



gfx::ppu::Coord ElementGrid::CalcContainerDimensions( ElementTileSize size )
{
	switch( size )
	{
		case ElementTileSize::Mini:
			return {
				kElementTilesetMini.mTileWidth *
					static_cast<gfx::ppu::CoordElem>( element::kNumElementLevels ),
				kElementTilesetMini.mTileHeight *
					static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };
		case ElementTileSize::Micro:
			return {
				kElementTilesetMicro.mTileWidth *
					static_cast<gfx::ppu::CoordElem>( element::kNumElementLevels ),
				kElementTilesetMicro.mTileHeight *
					static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };
		case ElementTileSize::Full:
		case ElementTileSize::Medium:
		case ElementTileSize::Invalid:
		default:
			RF_DBGFAIL();
			return {};
	}
}



void ElementGrid::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



void ElementGrid::UpdateFromCharacter( state::ObjectRef const& character )
{
	mCache.UpdateFromCharacter( character, false );
	UpdateDisplay();
}



void ElementGrid::UpdateFromCache( ElementGridDisplayCache const& cache )
{
	mCache = cache;
	UpdateDisplay();
}



void ElementGrid::OnInstanceAssign( UIContext& context, Container& container )
{
	gfx::ppu::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	ElementTilesetDef tilesetDef = {};
	switch( mSize )
	{
		case ElementTileSize::Mini:
			tilesetDef = kElementTilesetMini;
			break;
		case ElementTileSize::Micro:
			tilesetDef = kElementTilesetMicro;
			break;
		case ElementTileSize::Full:
		case ElementTileSize::Medium:
		case ElementTileSize::Invalid:
		default:
			RF_DBGFAIL();
	}

	RF_ASSERT_ASSUME( tilesetDef.mName != nullptr );
	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( tilesetDef.mName );

	if( tilesetDef.mUsesBorderSlots == false )
	{
		mTileLayer.ClearAndResize( element::kNumElementLevels, character::kMaxSlotsPerElementLevel );
	}
	else
	{
		// NOTE: Extra pseudo-slots on side for borders
		mTileLayer.ClearAndResize( element::kNumElementLevels + 1, character::kMaxSlotsPerElementLevel + 1 );
		for( size_t i_col = 0; i_col < element::kNumElementLevels; i_col++ )
		{
			mTileLayer.GetMutableTile( i_col, character::kMaxSlotsPerElementLevel ).mIndex =
				math::enum_bitcast( ElementTilesetIndex::TopBorder );
		}
		for( size_t i_row = 0; i_row < character::kMaxSlotsPerElementLevel; i_row++ )
		{
			mTileLayer.GetMutableTile( element::kNumElementLevels, i_row ).mIndex =
				math::enum_bitcast( ElementTilesetIndex::LeftBorder );
		}
		mTileLayer.GetMutableTile( element::kNumElementLevels, character::kMaxSlotsPerElementLevel ).mIndex =
			math::enum_bitcast( ElementTilesetIndex::TopLeftBorder );
	}

	RF_ASSERT( tilesetDef.mSupportsText == false );
}



void ElementGrid::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const expectedDimensions = CalcContainerDimensions( mSize );
	RF_ASSERT_MSG( container.mAABB.Width() == expectedDimensions.x, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == expectedDimensions.y, "Container not sized as needed" );

	gfx::ppu::Coord const pos = AlignToJustify( expectedDimensions, container.mAABB, mJustification );

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////

void ElementGrid::UpdateDisplay()
{
	for( element::ElementLevel const& level : element::kElementLevels )
	{
		size_t const levelOffset = element::AsLevelOffset( level );

		ElementGridDisplayCache::Column const& column = mCache.GetColumnRef( level );
		for( size_t i_row = 0; i_row < column.size(); i_row++ )
		{
			ElementGridDisplayCache::Slot const& slot = column.at( i_row );
			mTileLayer.GetMutableTile( levelOffset, i_row ).mIndex = math::enum_bitcast( slot.mTilesetIndex );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
