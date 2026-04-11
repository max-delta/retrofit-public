#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"
#include "GameUI/BorderFrameShape.h"

#include "PPU/TileLayer.h"

#include "core_math/Vector2.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Creates a backgraound and border using specialized 'frame tilesets' that
//  have special rules, see internal code for details on their requirements
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
		BorderFrameShape const& shape );
	void SetJustification( Justification::Value justification );

	// NOTE: Must have the same dimensions, will not perform a recalc
	void SwapPeerTileset( gfx::ManagedTilesetID tileset );

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
	BorderFrameShape mShape = {};
	gfx::ppu::Coord mExpectedDimensions = {};
	gfx::ppu::TileLayer mTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}
