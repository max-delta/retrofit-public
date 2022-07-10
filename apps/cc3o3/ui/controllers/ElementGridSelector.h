#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/ui/ElementGridDisplayCache.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/GenericListBox.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/TileLayer.h"


namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Requires a specifically-sized container
class ElementGridSelector final : public GenericListBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ElementGridSelector );

	//
	// Types and constants
public:
	static constexpr ElementTilesetDef kElementTilesetDef = kElementTilesetFull;


	//
	// Public methods
public:
	ElementGridSelector();

	static gfx::ppu::Coord CalcContainerDimensions();

	void UpdateFromCharacter( state::ObjectRef const& character );
	void UpdateFromCache( ElementGridDisplayCache const& cache );
	void DarkenAll();

	character::ElementSlotIndex GetSelectedIndex( UIConstContext const& context ) const;

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	virtual void PostInstanceAssign( UIContext& context, Container& container ) override;
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const override;
	virtual bool OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent ) override;

	WeakPtr<TextLabel> GetMutableSlotController( size_t slotIndex );

	void UpdateDisplay();


	//
	// Private data
private:
	element::ElementLevel mLevel = 0;
	gfx::ppu::TileLayer mTileLayer = {};
	ElementGridDisplayCache mCache = {};
};

///////////////////////////////////////////////////////////////////////////////
}
