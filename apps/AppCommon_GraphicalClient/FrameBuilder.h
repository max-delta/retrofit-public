#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core_logic/PartialPlanner.h"
#include "core/ptr/weak_ptr.h"

#include "rftl/string"
#include "rftl/vector"


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


	//
	// Public methods
public:
	FrameBuilder() = default;
	~FrameBuilder() = default;

	// Add tasks to the plan
	// NOTE: Cannot be called after finalization
	bool AddTask( TaskRepresentation && taskRepresentation, UniquePtr<scheduling::Task> cloneableTask );

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
	bool StartPlan( WeakPtr<scheduling::TaskPool> taskPool );

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
	void OnTaskComplete( TaskRef const& task );


	//
	// Private data
private:
	bool mFinalized = false;

	PartialPlanner mPartialPlanner;
	TaskHandles mTaskHandles;

	PartialPlanner::Preconditions mPreconditions;
	PartialPlanner::Postconditions mPostcontidions;

	PartialPlanner::PartialPlan mLastValidPlan;
	PartialPlanner::PartialPlan mCurrentPlan;
	WeakPtr<scheduling::TaskPool> mCurrentTaskPool;
};

///////////////////////////////////////////////////////////////////////////////
}}
