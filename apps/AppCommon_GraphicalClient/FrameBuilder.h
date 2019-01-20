#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core_logic/PartialPlanner.h"
#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/vector"
#include "rftl/mutex"


namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

class APPCOMMONGRAPHICALCLIENT_API FrameBuilder
{
	RF_NO_COPY( FrameBuilder );

	//
	// Types
public:
	using PartialPlanner = logic::PartialPlanner<rftl::string, rftl::string, scheduling::Task*>;
	using ConditionID = PartialPlanner::State::StateID;
	using ConditionValue = PartialPlanner::State::StateValue;
	using TaskRepresentation = PartialPlanner::Action;
	using TaskRef = TaskRepresentation::MetaValue;
private:
	using TaskHandles = rftl::vector<UniquePtr<scheduling::Task>>;
	using TasksInFlight = rftl::unordered_map<scheduling::TaskID, PartialPlanner::PartialPlan::PlannedActionID>;
	using PlannedActionsInFlight = rftl::unordered_set<PartialPlanner::PartialPlan::PlannedActionID>;


	//
	// Public methods
public:
	explicit FrameBuilder( WeakPtr<scheduling::TaskScheduler> const& scheduler );
	~FrameBuilder();

	// Add tasks to the plan
	// NOTE: Cannot be called after finalization
	bool AddTask( TaskRepresentation&& taskRepresentation, UniquePtr<scheduling::Task> cloneableTask );

	// Finalize the plan before starting, after which the plan can no longer be
	//  modified
	bool FinalizePlan();

	// Set/unset the preconditions and postconditions of the plan
	// NOTE: Cannot be called before finalization
	// NOTE: Cannot be called while a plan is in progress
	bool SetStartingCondition( ConditionID const& conditionID, ConditionValue const& conditionValue );
	bool RemoveStartingCondition( ConditionID const& conditionID );
	bool SetDesiredCondition( ConditionID const& conditionID, ConditionValue const& conditionValue );
	bool RemoveDesiredCondition( ConditionID const& conditionID );

	// Start executing the plan
	// NOTE: Cannot be called before finalization
	// NOTE: Cannot be called while a plan is in progress
	// NOTE: If called when the last plan is complete, will start a new
	//  instance of the plan
	bool StartPlan();

	// Check if the current plan is complete
	// NOTE: Cannot be called before finalization
	bool IsPlanComplete() const;


	//
	// Private methods
private:
	bool TestCanChangeConditions() const;
	void OnConditionsChanged();

	bool HasLastValidPlan() const;

	void OnPlanStart();
	void OnTaskComplete( TaskRef const& task, scheduling::TaskID taskID );
	void ScheduleNextTasks();

	void LogFlight( PartialPlanner::PartialPlan::PlannedActionID id );
	void LogReturn( PartialPlanner::PartialPlan::PlannedActionID id );
	void LogTrivial( PartialPlanner::PartialPlan::PlannedActionID id );


	//
	// Private data
private:
	bool mFinalized = false;

	UniquePtr<FrameBuilder*> mSelf;

	PartialPlanner mPartialPlanner;
	TaskHandles mTaskHandles;

	WeakPtr<scheduling::TaskScheduler> mScheduler;
	WeakPtr<scheduling::TaskPool> mTaskPool;

	PartialPlanner::Preconditions mPreconditions;
	PartialPlanner::Postconditions mPostconditions;

	PartialPlanner::PartialPlan mLastValidPlan;

	// NOTE: Task flighting lock applies to current plan and flight lists
	rftl::mutex mTaskFlightingLock;
	PartialPlanner::PartialPlan mCurrentPlan;
	TasksInFlight mTasksInFlight;
	PlannedActionsInFlight mPlannedActionsInFlight;
};

///////////////////////////////////////////////////////////////////////////////
}}
