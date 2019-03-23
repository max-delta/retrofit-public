#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/TileLayer.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API BorderFrame final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( BorderFrame );

	//
	// Public methods
public:
	BorderFrame() = default;

	void SetTileset( gfx::ManagedTilesetID tileset, gfx::PPUCoordElem expectedTileWidth, gfx::PPUCoordElem expectedTileHeight );
	void SetJustification( Justification justification );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private methods
private:
	void RecalcTilemap( Container const& container );

	//
	// Private data
private:
	Justification mJustification = Justification::MiddleCenter;
	gfx::PPUCoord mExpectedTileDimensions = {};
	gfx::PPUCoord mExpectedDimensions = {};
	gfx::TileLayer mTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}
