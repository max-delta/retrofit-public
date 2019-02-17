#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "rftl/string"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API TextLabel final : public Controller
{
	//
	// Public methods
public:
	void SetFont( gfx::ManagedFontID fontID, uint8_t desiredHeight );
	void SetText( char const* text );

	virtual void OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	gfx::ManagedFontID mFontID = gfx::kInvalidManagedFontID;
	uint8_t mDesiredHeight = 0;

	rftl::string mText;
};

///////////////////////////////////////////////////////////////////////////////
}}}
