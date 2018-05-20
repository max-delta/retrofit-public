#pragma once

// TODO: Templatize
#include "core_fsm/FiniteStateMachine.h"

#include "rftl/vector"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

class HierarchicalStateMachine
{
public:
	// TODO: Templatize
	using StateMachine = FiniteStateMachine;
	using State = FiniteStateMachine::State;
	using StateID = FiniteStateMachine::StateID;
	using StateCollection = FiniteStateMachine::StateCollection;
	using StateChangeContext = FiniteStateMachine::StateChangeContext;
	using Allocator = rftl::allocator<typename StateMachine>;
private:
	using StateMachineStack = rftl::vector<StateMachine, Allocator>;


public:
	explicit HierarchicalStateMachine( WeakPtr<StateCollection> const& stateCollection )
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
		RF_ASSERT( mCurrentStates.empty() );
		mCurrentStates.emplace_back( mStateCollection );
		StateMachine& newSubState = mCurrentStates.back();
		OnSubStateCreation( newSubState );
		return newSubState.Start( initialState );
	}


	void Stop()
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentStates.empty() == false );
		while( mCurrentStates.empty() == false )
		{
			mCurrentStates.back().Stop();
			mCurrentStates.pop_back();
		};
	}


	State* ChangeState( size_t level, StateID const& newState )
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentStates.empty() == false );
		RF_ASSERT( mCurrentStates.size() < level );
		while( mCurrentStates.size() > level + 1 )
		{
			mCurrentStates.back().Stop();
			mCurrentStates.pop_back();
		};
		return mCurrentStates.back().ChangeState( newState );
	}


	State* CreateSubState( StateID const& initialState )
	{
		RF_ASSERT( mStateCollection != nullptr );
		RF_ASSERT( mCurrentStates.empty() == false );
		mCurrentStates.emplace_back( mStateCollection );
		StateMachine& newSubState = mCurrentStates.back();
		OnSubStateCreation( newSubState );
		return newSubState.Start( initialState );
	}


protected:
	void OnSubStateCreation( StateMachine& newSubState ) const
	{
		//
	}


protected:
	WeakPtr<StateCollection> mStateCollection;
	StateMachineStack mCurrentStates;
};

///////////////////////////////////////////////////////////////////////////////
}}
