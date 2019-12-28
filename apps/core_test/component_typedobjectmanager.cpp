#include "stdafx.h"

#include "core_component/TypedObjectManager.h"
#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

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

TEST( TypedObjectManager, Decay )
{
	{
		TypedObjectRef<details::PrimitiveResolver> const typed;
		ObjectRef const untyped = typed;
		ASSERT_EQ( untyped, typed );
	}
	{
		TypedMutableObjectRef<details::PrimitiveResolver> const typed;
		MutableObjectRef const untyped = typed;
		ASSERT_EQ( untyped, typed );
	}
	{
		TypedMutableObjectRef<details::PrimitiveResolver> const mut;
		TypedObjectRef<details::PrimitiveResolver> const get = mut;
		ASSERT_EQ( get, mut );
	}

	{
		TypedComponentRef<details::PrimitiveResolver> const typed;
		ComponentRef const untyped = typed;
		ASSERT_EQ( untyped, typed );
	}
	{
		TypedMutableComponentRef<details::PrimitiveResolver> const typed;
		MutableComponentRef const untyped = typed;
		ASSERT_EQ( untyped, typed );
	}
	{
		TypedMutableComponentRef<details::PrimitiveResolver> const mut;
		TypedComponentRef<details::PrimitiveResolver> const get = mut;
		ASSERT_EQ( get, mut );
	}
}



TEST( TypedObjectManager, BasicAddRemoveObject )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;

	using Manager = TypedObjectManager<details::PrimitiveResolver>;
	using TypedObjectRef = TypedObjectRef<details::PrimitiveResolver>;

	Manager manager( kMan, kScope, details::PrimitiveResolver() );
	Manager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	TypedObjectRef const newObj = manager.AddObject();
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( Manager::GetManagerIdentifierFromObjectIdentifier( newObj.GetIdentifier() ), kMan );
	ASSERT_EQ( Manager::GetGenerationScopeFromObjectIdentifier( newObj.GetIdentifier() ), kScope );

	bool const validPostAdd = manager.IsValidObject( newObj.GetIdentifier() );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	{
		TypedObjectRef const viaGet = manager.GetObject( newObj.GetIdentifier() );
		ASSERT_EQ( viaGet, newObj );
		TypedMutableObjectRef const viaMut = manager.GetMutableObject( newObj.GetIdentifier() );
		ASSERT_EQ( viaMut, newObj );
	}

	bool const removeSuccess = manager.RemoveObject( newObj.GetIdentifier() );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( removeSuccess );
	latestState = manager.GetInternalStateIteration();

	bool const validPostRemove = manager.IsValidObject( newObj.GetIdentifier() );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_FALSE( validPostRemove );
}



TEST( TypedObjectManager, BasicAddRemoveComponent )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;

	using Manager = TypedObjectManager<details::PrimitiveResolver>;
	using TypedObjectRef = TypedObjectRef<details::PrimitiveResolver>;
	using CompType = int;
	static constexpr CompType kVal = 7;

	Manager manager( kMan, kScope, details::PrimitiveResolver() );
	TypedObjectRef const newObj = manager.AddObject();
	ObjectIdentifier const objID = newObj.GetIdentifier();
	Manager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	ComponentInstance newInstance = DefaultCreator<CompType>::Create( kVal );
	ComponentInstanceRef const newInstRef = newInstance;

	TypedMutableComponentRef const newComp = manager.AddComponentT<CompType>( objID, rftl::move( newInstance ) );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( newComp.GetObject().GetIdentifier(), objID );
	ASSERT_NE( newInstRef, nullptr );

	bool const validPostAdd = manager.IsValidComponentT<CompType>( objID );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	ComponentInstanceRefT<CompType> const instRef = manager.GetComponentInstanceT<CompType>( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_EQ( instRef, newComp.GetComponentInstance() );
	ASSERT_NE( instRef, nullptr );
	ASSERT_EQ( instRef, newInstRef );

	MutableComponentInstanceRefT<CompType> const mutInstRef = manager.GetMutableComponentInstanceT<CompType>( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_EQ( mutInstRef, newComp.GetMutableComponentInstance() );
	ASSERT_NE( mutInstRef, nullptr );
	ASSERT_EQ( mutInstRef, newInstRef );

	{
		TypedMutableObjectRef const mutObj = manager.GetMutableObject( newObj.GetIdentifier() );

		TypedComponentRef const viaGet = manager.GetComponentT<CompType>( newComp.GetObject().GetIdentifier() );
		ASSERT_EQ( viaGet, newComp );
		TypedMutableComponentRef const viaMut = manager.GetMutableComponentT<CompType>( newComp.GetObject().GetIdentifier() );
		ASSERT_EQ( viaMut, newComp );
		TypedComponentRef const viaObjGet = newObj.GetComponentT<CompType>();
		ASSERT_EQ( viaObjGet, newComp );
		TypedMutableComponentRef const viaObjMut = mutObj.GetComponentT<CompType>();
		ASSERT_EQ( viaObjMut, newComp );

		ComponentInstanceRefT<CompType> const viaGetCompGet = viaObjGet.GetComponentInstanceT<CompType>();
		ASSERT_EQ( viaGetCompGet, newInstRef );
		ComponentInstanceRefT<CompType> const viaMutCompGet = viaObjMut.GetComponentInstanceT<CompType>();
		ASSERT_EQ( viaMutCompGet, newInstRef );
		MutableComponentInstanceRefT<CompType> const viaMutCompMut = viaObjMut.GetMutableComponentInstanceT<CompType>();
		ASSERT_EQ( viaMutCompMut, newInstRef );
	}

	bool const removeSuccess = manager.RemoveComponentT<CompType>( objID );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( removeSuccess );
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
