#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/TileLayer.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API BorderFrame final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( BorderFrame );

	//
	// Public methods
public:
	BorderFrame() = default;

	void SetTileset(
		ui::UIContext& context,
		gfx::ManagedTilesetID tileset,
		gfx::ppu::Coord expectedTileDimensions,
		gfx::ppu::Coord expectedPatternDimensions,
		gfx::ppu::Coord paddingDimensions );
	void SetJustification( Justification::Value justification );

	ContainerID GetChildContainerID() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private methods
private:
	void RecalcTilemap( Container const& container );

	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mChildContainerID = kInvalidContainerID;
	Justification::Value mJustification = Justification::MiddleCenter;
	gfx::ppu::Coord mExpectedTileDimensions = {};
	gfx::ppu::Coord mExpectedPatternDimensions = {};
	gfx::ppu::Coord mPaddingDimensions = {};
	gfx::ppu::Coord mExpectedDimensions = {};
	gfx::ppu::TileLayer mTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}
