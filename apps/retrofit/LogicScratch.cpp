#include "stdafx.h"

#include "core_logic/PartialPlanner.h"

#include "rftl/string"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

void LogicScratch()
{
	constexpr bool kIncludeFailureCases = false;
	constexpr bool kIncludeUnimplementedCases = false;
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

	// NOP
	planner = {};
	{
		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', true );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 2 );
	}

	// Trivial single
	planner = {};
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 3 );
	}

	// Trivial double
	planner = {};
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+AB";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 3 );
	}

	// Independent pair
	planner = {};
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}
		{
			PartialPlanner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 4 );
	}

	// Dependent pair
	planner = {};
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+A-B";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			PartialPlanner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 4 );
	}

	// Mutually dependent pair
	planner = {};
	if( kIncludeFailureCases )
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+A-B";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			PartialPlanner::Action action;
			action.mMeta = "+B-A";
			action.mPostconditions.mStates.emplace( 'A', false );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess == false );
	}

	// Bad option
	planner = {};
	if( kIncludeUnimplementedCases )
	{
		{
			PartialPlanner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}
		{
			// If the planner tries this one it will fail
			PartialPlanner::Action action;
			action.mMeta = "+B-A";
			action.mPreconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'A', false );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}
		{
			// If the planner tries this one it will succeed
			PartialPlanner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		PartialPlanner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		PartialPlanner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
		PartialPlanner::PartialPlan const plan = planner.FetchPlan();
		RF_ASSERT( plan.mPlannedActions.size() == 4 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}