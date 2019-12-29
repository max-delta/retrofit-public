#include "stdafx.h"

#include "core_component/ObjectManager.h"
#include "core_component/ObjectRef.h"
#include "core_component/ComponentRef.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/void_creator.h"


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

	{
		ObjectRef const viaGet = manager.GetObject( newObj.GetIdentifier() );
		ASSERT_EQ( viaGet, newObj );
		MutableObjectRef const viaMut = manager.GetMutableObject( newObj.GetIdentifier() );
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



TEST( ObjectManager, BasicAddRemoveComponent )
{
	static constexpr ManagerIdentifier kMan = 1;
	static constexpr ScopeIdentifier kScope = 3;
	static constexpr ResolvedComponentType kCompType = 5;

	ObjectManager manager( kMan, kScope );
	ObjectRef const newObj = manager.AddObject();
	ObjectIdentifier const objID = newObj.GetIdentifier();
	ObjectManager::InternalStateIteration latestState = manager.GetInternalStateIteration();

	ComponentInstance newInstance = VoidCreator::Create( const_cast<void*>( compiler::kInvalidNonNullPointer ) );
	ComponentInstanceRef const newInstRef = newInstance;

	MutableComponentRef const newComp = manager.AddComponent( objID, kCompType, rftl::move( newInstance ) );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	latestState = manager.GetInternalStateIteration();
	ASSERT_EQ( newComp.GetObject().GetIdentifier(), objID );
	ASSERT_NE( newInstRef, nullptr );

	bool const validPostAdd = manager.IsValidComponent( objID, kCompType );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( validPostAdd );

	ComponentInstanceRef const instRef = manager.GetComponentInstance( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_EQ( instRef, newComp.GetComponentInstance() );
	ASSERT_NE( instRef, nullptr );
	ASSERT_EQ( instRef, newInstRef );

	MutableComponentInstanceRef const mutInstRef = manager.GetMutableComponentInstance( newComp );
	ASSERT_EQ( manager.GetInternalStateIteration(), latestState );
	ASSERT_EQ( mutInstRef, newComp.GetMutableComponentInstance() );
	ASSERT_NE( mutInstRef, nullptr );
	ASSERT_EQ( mutInstRef, newInstRef );

	{
		MutableObjectRef const mutObj = manager.GetMutableObject( newObj.GetIdentifier() );

		ComponentRef const viaGet = manager.GetComponent( newComp.GetObject().GetIdentifier(), newComp.GetComponentType() );
		ASSERT_EQ( viaGet, newComp );
		MutableComponentRef const viaMut = manager.GetMutableComponent( newComp.GetObject().GetIdentifier(), newComp.GetComponentType() );
		ASSERT_EQ( viaMut, newComp );
		ComponentRef const viaObjGet = newObj.GetComponent( newComp.GetComponentType() );
		ASSERT_EQ( viaObjGet, newComp );
		MutableComponentRef const viaObjMut = mutObj.GetComponent( newComp.GetComponentType() );
		ASSERT_EQ( viaObjMut, newComp );

		ComponentInstanceRef const viaGetCompGet = viaObjGet.GetComponentInstance();
		ASSERT_EQ( viaGetCompGet, newInstRef );
		ComponentInstanceRef const viaMutCompGet = viaObjMut.GetComponentInstance();
		ASSERT_EQ( viaMutCompGet, newInstRef );
		MutableComponentInstanceRef const viaMutCompMut = viaObjMut.GetMutableComponentInstance();
		ASSERT_EQ( viaMutCompMut, newInstRef );

		ComponentInstanceRef const viaGetObjInstGet = newObj.GetComponentInstance( newComp.GetComponentType() );
		ASSERT_EQ( viaGetObjInstGet, newInstRef );
		ComponentInstanceRef const viaMutObjInstGet = mutObj.GetComponentInstance( newComp.GetComponentType() );
		ASSERT_EQ( viaMutObjInstGet, newInstRef );
		MutableComponentInstanceRef const viaMutObjInstMut = mutObj.GetMutableComponentInstance( newComp.GetComponentType() );
		ASSERT_EQ( viaMutObjInstMut, newInstRef );
	}

	bool const removeSuccess = manager.RemoveComponent( objID, kCompType );
	ASSERT_NE( manager.GetInternalStateIteration(), latestState );
	ASSERT_TRUE( removeSuccess );
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
