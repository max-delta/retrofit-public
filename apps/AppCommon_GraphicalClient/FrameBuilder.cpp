#include "stdafx.h"
#include "FrameBuilder.h"

#include "Logging/Logging.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

bool FrameBuilder::AddTask( TaskRepresentation && taskRepresentation, UniquePtr<scheduling::Task> cloneableTask )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Cannot add task after finalization" );
		return false;
	}

	if( taskRepresentation.mMeta != cloneableTask )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Task representation differs from task handle" );
		return false;
	}

	if( taskRepresentation.mMeta == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_APPCOMMON, "Null task will be planned, but have no outward effect when executed" );
	}

	mPartialPlanner.AddActionToDatabase( taskRepresentation );
	mTaskHandles.emplace_back( rftl::move( cloneableTask ) );
	return true;
}



bool FrameBuilder::FinalizePlan()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Double finalization" );
		return false;
	}

	mFinalized = true;
	return true;
}



bool FrameBuilder::SetStartingCondition( ConditionID const & conditionID, ConditionValue const & conditionValue )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPreconditions.mStates.emplace( conditionID, conditionValue );
	return true;
}



bool FrameBuilder::RemoveStartingCondition( ConditionID const & conditionID )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPreconditions.mStates.erase( PartialPlanner::State( conditionID, ConditionValue() ) );
	return true;
}



bool FrameBuilder::SetDesiredCondition( ConditionID const & conditionID, ConditionValue const & conditionValue )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPostcontidions.mStates.emplace( conditionID, conditionValue );
	return true;
}



bool FrameBuilder::RemoveDesiredCondition( ConditionID const & conditionID )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPostcontidions.mStates.erase( PartialPlanner::State( conditionID, ConditionValue() ) );
	return true;
}



bool FrameBuilder::StartPlan( WeakPtr<scheduling::TaskPool> taskPool )
{
	RF_ASSERT( taskPool != nullptr );

	if( mFinalized == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Cannot start plan before finalization" );
		return false;
	}

	if( IsPlanComplete() == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Cannot start plan while plan is in progress" );
		return false;
	}

	if( HasLastValidPlan() == false )
	{
		// Need to plan/re-plan
		bool const planSuccess = mPartialPlanner.Run( nullptr, mPreconditions, nullptr, mPostcontidions );
		if( planSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Plan failed" );
			return false;
		}
		mLastValidPlan = mPartialPlanner.FetchPlan();
	}

	// Copy and use/re-use last valid plan
	RF_ASSERT( HasLastValidPlan() );
	mCurrentPlan = mLastValidPlan;

	// Start
	mCurrentTaskPool = taskPool;
	OnPlanStart();
	return true;
}



bool FrameBuilder::IsPlanComplete() const
{
	return mCurrentPlan.mPlannedActions.empty();
}

///////////////////////////////////////////////////////////////////////////////

bool FrameBuilder::TestCanChangeConditions() const
{
	if( mFinalized == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Cannot change conditions before finalization" );
		return false;
	}

	if( IsPlanComplete() == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_APPCOMMON, "Cannot change conditions while plan is in progress" );
		return false;
	}

	return true;
}



void FrameBuilder::OnConditionsChanged()
{
	// TODO: Destroy last valid plan to force re-plan
	// TODO: Log if this destruction took place
}



bool FrameBuilder::HasLastValidPlan() const
{
	return mLastValidPlan.mPlannedActions.empty() == false;
}



void FrameBuilder::OnPlanStart()
{
	// TODO: Pull first task, push to pool, wait
}



void FrameBuilder::OnTaskComplete( TaskRef const& task )
{
	// Remove task
}

///////////////////////////////////////////////////////////////////////////////
}}
