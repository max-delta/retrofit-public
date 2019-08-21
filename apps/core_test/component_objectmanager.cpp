#include "stdafx.h"

#include "core_component/ObjectManager.h"
#include "core_component/ObjectRef.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

TEST( ObjectManager, Empty )
{
	static constexpr ManagerIdentifier kMan = 1;

	ObjectManager const manager( kMan );

	ObjectManager::InternalStateIteration const initialState = manager.GetInternalStateIteration();

	ASSERT_EQ( manager.GetManagerIdentifier(), kMan );
	ASSERT_FALSE( manager.IsValidObject( kInvalidObjectIdentifier ) );
	ASSERT_FALSE( manager.IsValidComponent( kInvalidObjectIdentifier, kInvalidResolvedComponentType ) );

	ASSERT_EQ( manager.GetInternalStateIteration(), initialState );
}



TEST( ObjectManager, BasicAddRemoveObject )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;

	ObjectManager manager( kMan, kScope );
	ObjectManager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	ObjectRef const newObj = manager.AddObject();
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( ObjectManager::GetManagerIdentifierFromObjectIdentifier( newObj.GetIdentifier() ), kMan );
	ASSERT_EQ( ObjectManager::GetGenerationScopeFromObjectIdentifier( newObj.GetIdentifier() ), kScope );

	bool const validPostAdd = manager.IsValidObject( newObj.GetIdentifier() );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	bool const removeSuccess = manager.RemoveObject( newObj.GetIdentifier() );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();

	bool const validPostRemove = manager.IsValidObject( newObj.GetIdentifier() );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_FALSE( validPostRemove );
}

///////////////////////////////////////////////////////////////////////////////
}}
