#include "stdafx.h"

#include "core_component/ObjectManager.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

TEST( ObjectManager, Empty )
{
	ObjectManager const manager( 1 );

	ObjectManager::InternalStateIteration const initialState = manager.GetInternalStateIteration();

	ASSERT_EQ( manager.GetManagerIdentifier(), 1 );
	ASSERT_FALSE( manager.IsValidObject( kInvalidObjectIdentifier ) );
	ASSERT_FALSE( manager.IsValidComponent( kInvalidObjectIdentifier, kInvalidResolvedComponentType ) );

	ASSERT_EQ( manager.GetInternalStateIteration(), initialState );
}

///////////////////////////////////////////////////////////////////////////////
}}
