#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/InstancedController.h"

#include "PPU/TileLayer.h"


namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class ElementGrid final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ElementGrid );

	//
	// Types and constants
private:
	static constexpr char const kTilesetName[] = "elemgrid_8_16";
	static constexpr gfx::PPUCoordElem kTileWidth = 16;
	static constexpr gfx::PPUCoordElem kTileHeight = 8;

	//
	// Public methods
public:
	ElementGrid() = default;

	void SetJustification( Justification justification );
	void UpdateFromCharacter( state::ObjectRef const& character );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	Justification mJustification = Justification::MiddleCenter;
	gfx::TileLayer mTileLayer = {};
};

///////////////////////////////////////////////////////////////////////////////
}
