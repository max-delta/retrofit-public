#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/ui/ElementGridDisplayCache.h"
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
public:
	enum class Size : uint8_t
	{
		Mini = 0,
		Micro
	};

	static constexpr gfx::ppu::CoordElem kMiniContainerWidth = kElementTilesetMini.mTileWidth *
		static_cast<gfx::ppu::CoordElem>( character::kMaxElementLevels );
	static constexpr gfx::ppu::CoordElem kMiniContainerHeight = kElementTilesetMini.mTileHeight *
		static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel );

	static constexpr gfx::ppu::CoordElem kMicroContainerWidth = kElementTilesetMicro.mTileWidth *
		static_cast<gfx::ppu::CoordElem>( character::kMaxElementLevels );
	static constexpr gfx::ppu::CoordElem kMicroContainerHeight = kElementTilesetMicro.mTileHeight *
		static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel );


	//
	// Public methods
public:
	ElementGrid( Size size );

	void SetJustification( Justification::Value justification );
	void UpdateFromCharacter( state::ObjectRef const& character );
	void UpdateFromCache( ElementGridDisplayCache const& cache );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	Size const mSize = Size::Mini;
	Justification::Value mJustification = Justification::MiddleCenter;
	gfx::ppu::TileLayer mTileLayer = {};
	ElementGridDisplayCache mCache = {};
};

///////////////////////////////////////////////////////////////////////////////
}
