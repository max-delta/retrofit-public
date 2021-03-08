#include "stdafx.h"
#include "FrameBuilder.h"

#include "Logging/Logging.h"
#include "Scheduling/TaskScheduler.h"
#include "Scheduling/tasks/Task.h"
#include "Scheduling/taskpools/FIFOTaskPool.h"
#include "Scheduling/taskpools/TaskPoolListener.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

FrameBuilder::FrameBuilder( WeakPtr<scheduling::TaskScheduler> const& scheduler )
	: mScheduler( scheduler )
{
	RF_ASSERT( mScheduler != nullptr );

	// Create weakly-attached listener
	mSelf = DefaultCreator<FrameBuilder*>::Create( this );
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
	newPool->SetListener( rftl::move( listener ) );
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



bool FrameBuilder::AddTask( TaskRepresentation&& taskRepresentation, UniquePtr<scheduling::Task> cloneableTask )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Cannot add task after finalization" );
		return false;
	}

	if( taskRepresentation.mMeta != cloneableTask )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Task representation differs from task handle" );
		return false;
	}

	if( taskRepresentation.mMeta == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_FRAMEBUILDER, "Null task will be planned, but have no outward effect when executed" );
	}

	mPartialPlanner.AddActionToDatabase( taskRepresentation );
	mTaskHandles.emplace_back( rftl::move( cloneableTask ) );
	return true;
}



bool FrameBuilder::FinalizePlan()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Double finalization" );
		return false;
	}

	mFinalized = true;
	return true;
}



bool FrameBuilder::SetStartingCondition( ConditionID const& conditionID, ConditionValue const& conditionValue )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPreconditions.mStates.emplace( conditionID, conditionValue );
	OnConditionsChanged();
	return true;
}



bool FrameBuilder::RemoveStartingCondition( ConditionID const& conditionID )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPreconditions.mStates.erase( PartialPlanner::State( conditionID, ConditionValue() ) );
	OnConditionsChanged();
	return true;
}



bool FrameBuilder::SetDesiredCondition( ConditionID const& conditionID, ConditionValue const& conditionValue )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPostconditions.mStates.emplace( conditionID, conditionValue );
	OnConditionsChanged();
	return true;
}



bool FrameBuilder::RemoveDesiredCondition( ConditionID const& conditionID )
{
	if( TestCanChangeConditions() == false )
	{
		return false;
	}

	mPostconditions.mStates.erase( PartialPlanner::State( conditionID, ConditionValue() ) );
	OnConditionsChanged();
	return true;
}



bool FrameBuilder::StartPlan()
{
	if( mFinalized == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Cannot start plan before finalization" );
		return false;
	}

	if( IsPlanComplete() == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Cannot start plan while plan is in progress" );
		return false;
	}

	if( HasLastValidPlan() == false )
	{
		// Need to plan/re-plan
		bool const planSuccess = mPartialPlanner.Run( nullptr, mPreconditions, nullptr, mPostconditions );
		if( planSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Plan failed" );
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
	rftl::lock_guard<rftl::mutex> lock{ mTaskFlightingLock };

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
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Cannot change conditions before finalization" );
		return false;
	}

	if( IsPlanComplete() == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEBUILDER, "Cannot change conditions while plan is in progress" );
		return false;
	}

	return true;
}



void FrameBuilder::OnConditionsChanged()
{
	if( HasLastValidPlan() )
	{
		mLastValidPlan = PartialPlanner::PartialPlan();
		RFLOG_WARNING( nullptr, RFCAT_FRAMEBUILDER, "Condition change caused a replan" );
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
		mCurrentPlan.mPlannedActions.erase( planIter );
		RF_ASSERT( mPlannedActionsInFlight.count( plannedID ) == 1 );
		mPlannedActionsInFlight.erase( plannedID );
		LogReturn( plannedID );
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

			// Check for already flighted actions
			if( mPlannedActionsInFlight.count( plannedActionID ) != 0 )
			{
				planIter++;
				continue;
			}

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
				LogTrivial( plannedActionID );
				planIter = mCurrentPlan.mPlannedActions.erase( planIter );
				continue;
			}

			// Clone
			scheduling::TaskPtr newTask = task->Clone();
			if( newTask == nullptr )
			{
				RFLOG_FATAL( nullptr, RFCAT_FRAMEBUILDER, "Failed to clone task" );
			}

			// Flight
			LogFlight( plannedActionID );
			RF_ASSERT( mTaskPool != nullptr );
			scheduling::TaskID const newTaskID = mTaskPool->AddTask( rftl::move( newTask ) );
			mTasksInFlight[newTaskID] = plannedActionID;
			mPlannedActionsInFlight.emplace( plannedActionID );

			planIter++;
			continue;
		}
	}
}



void FrameBuilder::LogFlight( PartialPlanner::PartialPlan::PlannedActionID id )
{
	RFLOG_TRACE( nullptr, RFCAT_FRAMEBUILDER, "Frame flight: %llu", id );
}



void FrameBuilder::LogReturn( PartialPlanner::PartialPlan::PlannedActionID id )
{
	RFLOG_TRACE( nullptr, RFCAT_FRAMEBUILDER, "Frame return: %llu", id );
}



void FrameBuilder::LogTrivial( PartialPlanner::PartialPlan::PlannedActionID id )
{
	RFLOG_TRACE( nullptr, RFCAT_FRAMEBUILDER, "Frame trivial: %llu", id );
}

///////////////////////////////////////////////////////////////////////////////
}
