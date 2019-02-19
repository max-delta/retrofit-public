#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "rftl/unordered_map"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API Font
{
	gfx::ManagedFontID mManagedFontID = gfx::kInvalidManagedFontID;
	uint8_t mFontHeight = 0;
	uint8_t mMinimumZoomFactor = 0;
};

///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FontRegistry
{
	//
	// Types and constants
public:
	using PurposeID = uint16_t;
	using FontsByZoomFactor = rftl::vector<Font>;
	using FontsByPurpose = rftl::unordered_map<PurposeID, FontsByZoomFactor>;


	//
	// Public methods
public:
	void RegisterFallbackFont( Font const& font );
	void RegisterFont( PurposeID purpose, Font const& font );

	Font SelectBestFont( PurposeID purpose, uint8_t zoomFactor ) const;


	//
	// Private data
private:
	FontsByPurpose mFontsByPurpose;
	Font mFallbackFont;
};

///////////////////////////////////////////////////////////////////////////////
}}
