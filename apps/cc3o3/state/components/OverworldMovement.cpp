#include "stdafx.h"
#include "OverworldMovement.h"

#include "core_math/Vector2.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void OverworldMovement::Pos::Bind( rollback::Window& window, state::VariableIdentifier const& parent, alloc::Allocator& allocator )
{
	mX.Bind( window, parent.GetChild( "x" ), allocator );
	mY.Bind( window, parent.GetChild( "y" ), allocator );
	mPrimary.Bind( window, parent.GetChild( "primary" ), allocator );
	mSecondary.Bind( window, parent.GetChild( "secondary" ), allocator );
	mLatent.Bind( window, parent.GetChild( "latent" ), allocator );
	mMoving.Bind( window, parent.GetChild( "moving" ), allocator );
}



phys::Coord OverworldMovement::Pos::GetCoord() const
{
	return phys::Coord( mX, mY );
}



void OverworldMovement::Pos::SetCoord( phys::Coord pos )
{
	mX = pos.x;
	mY = pos.y;
}

///////////////////////////////////////////////////////////////////////////////

void OverworldMovement::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	VariableIdentifier const compRoot = parent.GetChild( "overworld", "movement" );
	mCurPos.Bind( sharedWindow, compRoot, mAlloc );
}

///////////////////////////////////////////////////////////////////////////////
}
