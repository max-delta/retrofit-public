#include "stdafx.h"
#include "Anchor.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

void Anchor::InvalidatePosition()
{
	mPos = { Anchor::kInvalidPosElem, Anchor::kInvalidPosElem };
}



bool Anchor::HasValidPosition() const
{
	return
		mPos.x != Anchor::kInvalidPosElem &&
		mPos.y != Anchor::kInvalidPosElem;
}

///////////////////////////////////////////////////////////////////////////////
}
