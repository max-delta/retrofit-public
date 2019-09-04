#include "stdafx.h"

#include "core_component/TypedObjectManager.h"
#include "core_component/ObjectRef.h"
#include "core_component/ComponentRef.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct PrimitiveResolver
{
};

}
///////////////////////////////////////////////////////////////////////////////

TEST( TypedObjectManager, BasicAddRemoveComponent )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;
	static constexpr ResolvedComponentType kCompType = 5;

	using Manager = TypedObjectManager<details::PrimitiveResolver>;

	Manager manager( kMan, kScope, details::PrimitiveResolver() );
	ObjectRef const newObj = manager.AddObject();
	ObjectIdentifier const objID = newObj.GetIdentifier();
	Manager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	Manager::ComponentInstance newInstance = DefaultCreator<int>::Create( 7 );
	Manager::ComponentInstanceRef const newInstRef = newInstance;

	MutableComponentRef const newComp = manager.AddComponent( objID, kCompType, rftl::move( newInstance ) );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( newComp.GetObject().GetIdentifier(), objID );
	ASSERT_NE( newInstRef, nullptr );

	bool const validPostAdd = manager.IsValidComponent( objID, kCompType );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	Manager::ComponentInstanceRef const instRef = manager.GetComponentInstance( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_NE( instRef, nullptr );
	ASSERT_EQ( instRef, newInstRef );

	Manager::MutableComponentInstanceRef const mutInstRef = manager.GetMutableComponentInstance( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_NE( mutInstRef, nullptr );
	ASSERT_EQ( mutInstRef, newInstRef );

	bool const removeSuccess = manager.RemoveComponent( objID, kCompType );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();

	bool const validPostRemove = manager.IsValidComponent( objID, kCompType );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_FALSE( validPostRemove );

	ASSERT_EQ( newInstRef, nullptr );
	ASSERT_EQ( instRef, nullptr );
	ASSERT_EQ( mutInstRef, nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}}
