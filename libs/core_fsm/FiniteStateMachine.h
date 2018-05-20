#pragma once

// TODO: Templatize
#include "core_fsm/GenericStateCollection.h"
#include "core_fsm/VoidState.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

template<
	typename TState = VoidState,
	typename TStateID = nullptr_t,
	typename TStateCollection = GenericStateCollection<typename TStateID, typename TState>,
	typename TStateChangeContext = nullptr_t>
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


	State * Start( StateID const& initialState )
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
	StateChangeContext CreateStateChangeContext() const
	{
		return StateChangeContext{};
	}


protected:
	WeakPtr<StateCollection> mStateCollection;
	State* mCurrentState = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}}
