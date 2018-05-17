#include "stdafx.h"
#include "FrameBuilder.h"

#include "Logging/Logging.h"
#include "Scheduling/TaskScheduler.h"
#include "Scheduling/tasks/Task.h"
#include "Scheduling/taskpools/FIFOTaskPool.h"
#include "Scheduling/taskpools/TaskPoolListener.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/entwined_creator.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

FrameBuilder::FrameBuilder( WeakPtr<scheduling::TaskScheduler> const& scheduler )
	: mScheduler( scheduler )
{
	RF_ASSERT( mScheduler != nullptr );

	// Create weakly-attached listener
	mSelf = EntwinedCreator<FrameBuilder*>::Create( this );
	WeakPtr<FrameBuilder*> weakSelf = mSelf;
	scheduling::TaskPoolListener listener;
	auto onTaskRemoved = [weakSelf](
		scheduling::TaskPtr && task, scheduling::TaskID taskId, scheduling::TaskState newState ) -> void
	{
		FrameBuilder* const* const pSelf = weakSelf;
		if( pSelf != nullptr )
		{
			FrameBuilder* const self = *pSelf;
			RF_ASSERT( self != nullptr );
			self->OnTaskComplete( task, taskId );
		}
	};
	listener.mOnTaskRemoved = onTaskRemoved;

	// Create and register pool with listener
	UniquePtr<scheduling::FIFOTaskPool> newPool = DefaultCreator<scheduling::FIFOTaskPool>::Create();
	mTaskPool = scheduler->RegisterPool( rftl::move( newPool ), scheduling::TaskPriority::Normal );
	RF_ASSERT( mTaskPool != nullptr );
}



FrameBuilder::~FrameBuilder()
{
	// Unhook anyone with weak pointers back to us, such as the listener, before
	//  we proceed with destruction
	mSelf = nullptr;

	RF_ASSERT( mScheduler != nullptr );
	RF_ASSERT( mTaskPool != nullptr );
	mScheduler->UnregisterPool( mTaskPool );
}



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



bool FrameBuilder::StartPlan()
{
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
	OnPlanStart();
	return true;
}



bool FrameBuilder::IsPlanComplete() const
{
	if( mCurrentPlan.mPlannedActions.empty() )
	{
		RF_ASSERT( mTasksInFlight.empty() );
		return true;
	}
	return false;
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
	if( HasLastValidPlan() )
	{
		mLastValidPlan = PartialPlanner::PartialPlan();
		RFLOG_WARNING( nullptr, RFCAT_APPCOMMON, "Condition change caused a replan" );
	}
}



bool FrameBuilder::HasLastValidPlan() const
{
	return mLastValidPlan.mPlannedActions.empty() == false;
}



void FrameBuilder::OnPlanStart()
{
	ScheduleNextTasks();
}



void FrameBuilder::OnTaskComplete( TaskRef const& task, scheduling::TaskID taskID )
{
	// Remove task
	{
		rftl::lock_guard<rftl::mutex> lock{ mTaskFlightingLock };

		TasksInFlight::const_iterator const taskIter = mTasksInFlight.find( taskID );
		RF_ASSERT( taskIter != mTasksInFlight.end() );
		PartialPlanner::PartialPlan::PlannedActionID const plannedID = taskIter->second;
		mTasksInFlight.erase( taskIter );
		PartialPlanner::PartialPlan::PlannedActionIDMap::const_iterator const planIter = mCurrentPlan.mPlannedActions.find( plannedID );
		RF_ASSERT( planIter != mCurrentPlan.mPlannedActions.end() );
		RF_ASSERT( planIter->second == task );
		mCurrentPlan.mPlannedActions.erase( planIter );
	}

	ScheduleNextTasks();
}



void FrameBuilder::ScheduleNextTasks()
{
	rftl::lock_guard<rftl::mutex> lock{ mTaskFlightingLock };

	// Push new tasks
	// NOTE: Empty tasks execute immediately, so we have to re-evaluate when
	//  that happens
	bool hasExecutedEmptyTasks = true;
	while( hasExecutedEmptyTasks )
	{
		hasExecutedEmptyTasks = false;
		PartialPlanner::PartialPlan::PlannedActionIDMap::const_iterator planIter = mCurrentPlan.mPlannedActions.begin();
		while( planIter != mCurrentPlan.mPlannedActions.end() )
		{
			PartialPlanner::PartialPlan::PlannedActionID const& plannedActionID = planIter->first;
			TaskRef const& task = planIter->second;

			// Check for unmet dependencies
			bool dependenciesMet = true;
			PartialPlanner::PartialPlan::PlannedActionDependencyMap::const_iterator const depIter = mCurrentPlan.mDependencies.find( plannedActionID );
			if( depIter != mCurrentPlan.mDependencies.end() )
			{
				for( PartialPlanner::PartialPlan::PlannedActionID const& dep : depIter->second )
				{
					if( mCurrentPlan.mPlannedActions.count( dep ) != 0 )
					{
						dependenciesMet = false;
						break;
					}
				}
			}
			if( dependenciesMet == false )
			{
				planIter++;
				continue;
			}

			if( task == nullptr )
			{
				// Empty, remove
				hasExecutedEmptyTasks = true;
				planIter = mCurrentPlan.mPlannedActions.erase( planIter );
				continue;
			}

			// Clone
			scheduling::TaskPtr newTask = task->Clone();
			if( newTask == nullptr )
			{
				RFLOG_FATAL( nullptr, RFCAT_APPCOMMON, "Failed to clone task" );
			}

			// Flight
			RF_ASSERT( mTaskPool != nullptr );
			scheduling::TaskID const newTaskID = mTaskPool->AddTask( rftl::move( newTask ) );
			mTasksInFlight[newTaskID] = plannedActionID;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
