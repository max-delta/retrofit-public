#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"
#include "GameUI/Font.h"

#include "rftl/unordered_map"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FontRegistry
{
	//
	// Types and constants
public:
	using FontsByZoomFactor = rftl::vector<Font>;
	using FontsByPurpose = rftl::unordered_map<FontPurposeID, FontsByZoomFactor>;


	//
	// Public methods
public:
	void RegisterFallbackFont( Font const& font );
	void RegisterFont( FontPurposeID purpose, Font const& font );

	Font SelectBestFont( FontPurposeID purpose, uint8_t zoomFactor ) const;


	//
	// Private data
private:
	FontsByPurpose mFontsByPurpose;
	Font mFallbackFont;
};

///////////////////////////////////////////////////////////////////////////////
}
