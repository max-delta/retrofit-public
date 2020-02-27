#include "stdafx.h"
#include "Progression.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void Progression::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	mStoryTier.Bind( sharedWindow, parent.GetChild( "story_tier" ), mAlloc );
}

///////////////////////////////////////////////////////////////////////////////
}
