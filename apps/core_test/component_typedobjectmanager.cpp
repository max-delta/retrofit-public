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
	template<typename T>
	component::ResolvedComponentType operator()() const;

	bool operator()( ComponentRef const& from, ResolvedComponentType to ) const
	{
		return from.GetComponentType() == to;
	}
};

template<>
component::ResolvedComponentType PrimitiveResolver::operator()<int>() const
{
	return 51;
}

}
///////////////////////////////////////////////////////////////////////////////

TEST( TypedObjectManager, BasicAddRemoveComponent )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;

	using Manager = TypedObjectManager<details::PrimitiveResolver>;
	using CompType = int;
	static constexpr CompType kVal = 7;

	Manager manager( kMan, kScope, details::PrimitiveResolver() );
	ObjectRef const newObj = manager.AddObject();
	ObjectIdentifier const objID = newObj.GetIdentifier();
	Manager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	Manager::ComponentInstance newInstance = DefaultCreator<CompType>::Create( kVal );
	Manager::ComponentInstanceRef const newInstRef = newInstance;

	MutableComponentRef const newComp = manager.AddComponentT<CompType>( objID, rftl::move( newInstance ) );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( newComp.GetObject().GetIdentifier(), objID );
	ASSERT_NE( newInstRef, nullptr );

	bool const validPostAdd = manager.IsValidComponentT<CompType>( objID );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	Manager::ComponentInstanceRef const instRef = manager.GetComponentInstanceT<CompType>( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_NE( instRef, nullptr );
	ASSERT_EQ( instRef, newInstRef );

	Manager::MutableComponentInstanceRef const mutInstRef = manager.GetMutableComponentInstanceT<CompType>( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_NE( mutInstRef, nullptr );
	ASSERT_EQ( mutInstRef, newInstRef );

	bool const removeSuccess = manager.RemoveComponentT<CompType>( objID );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();

	bool const validPostRemove = manager.IsValidComponentT<CompType>( objID );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_FALSE( validPostRemove );

	ASSERT_EQ( newInstRef, nullptr );
	ASSERT_EQ( instRef, nullptr );
	ASSERT_EQ( mutInstRef, nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}}
