#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/TileLayer.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API TileLayerDisplay final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( TileLayerDisplay );

	//
	// Public methods
public:
	TileLayerDisplay() = default;

	void SetTileset( gfx::ManagedTilesetID tileset );
	gfx::TileLayer& GetMutableTileLayer();

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	gfx::TileLayer mTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}
