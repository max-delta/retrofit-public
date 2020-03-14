#include "stdafx.h"
#include "UINavigation.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void UINavigation::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	mPauseMenu.mNavigationDepth.Bind( privateWindow, parent.GetChild( "pause_menu", "navigation_depth" ), mAlloc );
	mPauseMenu.mTopLevelCursor.Bind( privateWindow, parent.GetChild( "pause_menu", "top_level_cursor" ), mAlloc );
}

///////////////////////////////////////////////////////////////////////////////
}
