#include "stdafx.h"
#include "OverworldMovement.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

void OverworldMovement::Pos::Bind( rollback::Window& window, state::VariableIdentifier const& parent, alloc::Allocator& allocator )
{
	mX.Bind( window, parent.GetChild( "x" ), allocator );
	mY.Bind( window, parent.GetChild( "y" ), allocator );
	mFacing.Bind( window, parent.GetChild( "facing" ), allocator );
	mMoving.Bind( window, parent.GetChild( "moving" ), allocator );
}



phys::PhysCoord OverworldMovement::Pos::GetCoord() const
{
	return phys::PhysCoord( mX, mY );
}



void OverworldMovement::Pos::SetCoord( phys::PhysCoord pos )
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
