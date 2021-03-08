#include "stdafx.h"

#include "core_logic/PartialPlanner.h"

#include "rftl/string"


namespace RF::logic {
///////////////////////////////////////////////////////////////////////////////
namespace partial_planner_tests {
constexpr bool kIncludeFailureCases = false;
constexpr bool kIncludeUnimplementedCases = false;
}

TEST( LogicPartialPlanner, Nop )
{
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

	PartialPlanner::Preconditions initialConditions;
	initialConditions.mStates.emplace( 'A', true );
	PartialPlanner::Postconditions desiredFinalConditions;
	desiredFinalConditions.mStates.emplace( 'A', true );
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 2 );
}



TEST( LogicPartialPlanner, TrivialSingle )
{
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 3 );
}



TEST( LogicPartialPlanner, TrivialDouble )
{
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 3 );
}



TEST( LogicPartialPlanner, IndependentPair )
{
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 4 );
}



TEST( LogicPartialPlanner, DependentPair )
{
	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 4 );
}



TEST( LogicPartialPlanner, BadOption )
{
	if( partial_planner_tests::kIncludeUnimplementedCases == false )
	{
		return;
	}

	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_TRUE( planSuccess );
	PartialPlanner::PartialPlan const plan = planner.FetchPlan();
	ASSERT_EQ( plan.mPlannedActions.size(), 4 );
}



TEST( LogicPartialPlanner, MutuallyDependentPair )
{
	if( partial_planner_tests::kIncludeFailureCases == false )
	{
		return;
	}

	using PartialPlanner = logic::PartialPlanner<char, bool, rftl::string>;
	PartialPlanner planner;

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
	bool const planSuccess = planner.Run( "Initial", initialConditions, "Final", desiredFinalConditions );
	ASSERT_FALSE( planSuccess );
}

///////////////////////////////////////////////////////////////////////////////
}
