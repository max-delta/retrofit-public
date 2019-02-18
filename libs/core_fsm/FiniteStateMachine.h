#pragma once

// TODO: Templatize
#include "core_fsm/GenericStateCollection.h"
#include "core_fsm/VoidState.h"

#include "core/ptr/weak_ptr.h"
#include "core/meta/DefaultConstruct.h"
#include "core/meta/ConstructorOverload.h"

#include "rftl/type_traits"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

template<
	typename TState = VoidState,
	typename TStateID = nullptr_t,
	typename TStateChangeContext = nullptr_t,
	typename TStateCollection = GenericStateCollection<typename TStateID, typename TState>>
class FiniteStateMachine
{
public:
	using State = TState;
	using StateID = TStateID;
	using StateCollection = TStateCollection;
	using StateChangeContext = TStateChangeContext;


public:
	explicit FiniteStateMachine( WeakPtr<StateCollection> const& stateCollection )
		: mStateCollection( stateCollection )
	{
		RF_ASSERT( mStateCollection != nullptr );
	}

	virtual ~FiniteStateMachine() = default;

	State const* LookupState( StateID const& identifier ) const
	{
		RF_ASSERT( mStateCollection != nullptr );
		return mStateCollection->LookupState( identifier );
	}


	State* LookupMutableState( StateID const& identifier )
	{
		RF_ASSERT( mStateCollection != nullptr );
		return mStateCollection->LookupMutableState( identifier );
	}


	State* Start( StateID const& initialState )
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentState == nullptr );
		mCurrentState = LookupMutableState( initialState );
		RF_ASSERT( mCurrentState != nullptr );
		StateChangeContext context = CreateStateChangeContext();
		mCurrentState->OnEnter( context );
		return mCurrentState;
	}


	void Stop()
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentState != nullptr );
		StateChangeContext context = CreateStateChangeContext();
		mCurrentState->OnExit( context );
		mCurrentState = nullptr;
	}


	State* ChangeState( StateID const& newState )
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentState != nullptr );
		StateChangeContext context = CreateStateChangeContext();
		mCurrentState->OnExit( context );
		mCurrentState = LookupMutableState( newState );
		RF_ASSERT( mCurrentState != nullptr );
		mCurrentState->OnEnter( context );
		return mCurrentState;
	}


protected:
	// Derived classes may choose to defer creating the state collection
	explicit FiniteStateMachine( RF::ExplicitDefaultConstruct )
		: mStateCollection( nullptr )
	{
		//
	}
	void SetStateCollection( WeakPtr<StateCollection> const& stateCollection )
	{
		RF_ASSERT( mStateCollection == nullptr );
		mStateCollection = stateCollection;
		RF_ASSERT( mStateCollection != nullptr );
	}

	State const* GetCurrentState() const
	{
		return mCurrentState;
	}

	State* GetCurrentMutableState()
	{
		return mCurrentState;
	}

	virtual StateChangeContext CreateStateChangeContext() = 0;


private:
	WeakPtr<StateCollection> mStateCollection;
	State* mCurrentState = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

template<
	typename TState = VoidState,
	typename TStateID = nullptr_t,
	typename TStateCollection = GenericStateCollection<typename TStateID, typename TState>>
class BasicFiniteStateMachine : public FiniteStateMachine<TState, TStateID, nullptr_t, TStateCollection>
{
public:
	explicit BasicFiniteStateMachine( WeakPtr<typename BasicFiniteStateMachine::StateCollection> const& stateCollection )
		: BasicFiniteStateMachine( stateCollection )
	{
		//
	}

private:
	virtual nullptr_t CreateStateChangeContext() override final
	{
		return nullptr;
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
