#include "stdafx.h"
#include "PPUState.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

void PPUState::Clear()
{
	mNumObjects = 0;
	mNumTileLayers = 0;
	mNumStrings = 0;
	mTextStorage.Clear();
}

///////////////////////////////////////////////////////////////////////////////
}
