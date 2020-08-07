#include "stdafx.h"
#include "UINavigation.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void UINavigation::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	mPauseMenu.mNavigationDepth.Bind( privateWindow, parent.GetChild( "pause_menu", "navigation_depth" ), mAlloc );
	mPauseMenu.mTopLevelCursor.Bind( privateWindow, parent.GetChild( "pause_menu", "top_level_cursor" ), mAlloc );

	mBattleMenu.mControlCharIndex.Bind( privateWindow, parent.GetChild( "battle_menu", "control_char" ), mAlloc );
	mBattleMenu.mControlState.Bind( privateWindow, parent.GetChild( "battle_menu", "control_mode" ), mAlloc );
	for( size_t i = 0; i < mBattleMenu.mCursorIndex.size(); i++ )
	{
		mBattleMenu.mCursorIndex.at( i ).Bind( privateWindow, parent.GetChild( "battle_menu", "cursor_index", rftl::to_string( i ) ), mAlloc );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
