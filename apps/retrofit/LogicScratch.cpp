#include "stdafx.h"

#include "core_math/Hash.h"
#include "core/macros.h"

#include "rftl/unordered_set"
#include "rftl/unordered_map"
#include "rftl/deque"
#include "rftl/string"
#include "rftl/limits"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

struct Planner
{
	void FAIL()
	{
		abort();
	}

	using StateID = char;
	using StateValue = bool;
	using State = rftl::pair<StateID, StateValue>;

	using Preconditions = rftl::unordered_set<State, math::RawBytesHash<State>>;
	using Postconditions = rftl::unordered_set<State, math::RawBytesHash<State> >;

	using ActionID = uint64_t;
	static constexpr ActionID kInvalidActionID = 0;
	static constexpr ActionID kReservedInitialActionID = 1;
	static constexpr ActionID kReservedFinalActionID = 2;
	static constexpr ActionID kFirstGenerateableActionID = 3;
	struct Action
	{
		rftl::string mDebugName;
		Preconditions mPreconditions;
		Postconditions mPostconditions;
	};
	struct ActionDatabase
	{
		using ActionsByID = rftl::unordered_map<ActionID, Action>;
		ActionsByID mActionsByID;
	};
	ActionDatabase actionDatabase;
	ActionID actionIDGenerator = kFirstGenerateableActionID;
	ActionID AddActionToDatabase( Action action )
	{
		ActionID const newActionID = actionIDGenerator++;
		actionDatabase.mActionsByID[newActionID] = action;
		return newActionID;
	}
	void AddReservedActionToDatabase( Action action, ActionID reservedActionID )
	{
		RF_ASSERT( reservedActionID == kReservedInitialActionID || reservedActionID == kReservedFinalActionID );
		actionDatabase.mActionsByID[reservedActionID] = action;
	}
	Action const& LookupActionInDatabase( ActionID actionID )
	{
		return actionDatabase.mActionsByID.at( actionID );
	}
	ActionID FindActionWithPostConditionInDatabase( State const& desiredPostCondition )
	{
		for( ActionDatabase::ActionsByID::value_type const& actionPair : actionDatabase.mActionsByID )
		{
			ActionID const actionID = actionPair.first;
			Action const& action = actionPair.second;
			Postconditions const& postConditions = action.mPostconditions;
			for( State const& condition : postConditions )
			{
				if( condition == desiredPostCondition )
				{
					return actionID;
				}
			}
		}

		return kInvalidActionID;
	}

	using PlannedActionInstanceID = uint64_t;
	static constexpr PlannedActionInstanceID kInvalidPlannedActionInstanceID = 0;
	using PlannedActionInstances = rftl::unordered_map<PlannedActionInstanceID, ActionID>;
	PlannedActionInstances plannedActionInstances;
	PlannedActionInstanceID plannedActionInstanceIDGenerator = kInvalidPlannedActionInstanceID;
	PlannedActionInstanceID PlanActionInstance( ActionID actionID )
	{
		PlannedActionInstanceID const newPlannedActionInstanceID = ++plannedActionInstanceIDGenerator;
		plannedActionInstances[newPlannedActionInstanceID] = actionID;
		return newPlannedActionInstanceID;
	}
	PlannedActionInstances const& GetPlannedActionInstances()
	{
		return plannedActionInstances;
	}
	ActionID const& LookupPlannedActionByInstanceID( PlannedActionInstanceID instanceID )
	{
		return plannedActionInstances.at( instanceID );
	}
	PlannedActionInstanceID FindPlannedActionInstanceWithPostCondition( State const& desiredPostCondition )
	{
		for( PlannedActionInstances::value_type const& actionPair : plannedActionInstances )
		{
			PlannedActionInstanceID const instanceID = actionPair.first;
			ActionID const actionID = actionPair.second;
			Action const& action = LookupActionInDatabase( actionID );
			Postconditions const& postConditions = action.mPostconditions;
			for( State const& condition : postConditions )
			{
				if( condition == desiredPostCondition )
				{
					return instanceID;
				}
			}
		}

		return kInvalidPlannedActionInstanceID;
	}

	using UnmetPlannedActionNeed = rftl::pair<PlannedActionInstanceID, State>;
	using UnmetPlannedActionNeeds = rftl::unordered_set<UnmetPlannedActionNeed, math::RawBytesHash<UnmetPlannedActionNeed> >;
	UnmetPlannedActionNeeds unmetPlannedActionNeeds;
	void AddNeedForUnmetPlannedAction( PlannedActionInstanceID actionID, State reason )
	{
		unmetPlannedActionNeeds.emplace( actionID, reason );
	}
	bool HasUnmetNeeds()
	{
		return unmetPlannedActionNeeds.empty() == false;
	}
	UnmetPlannedActionNeed PopUnmetNeed()
	{
		UnmetPlannedActionNeeds::const_iterator iter = unmetPlannedActionNeeds.begin();
		UnmetPlannedActionNeed const retVal = *iter;
		unmetPlannedActionNeeds.erase( iter );
		return retVal;
	}

	struct CausalLink
	{
		PlannedActionInstanceID mFullfillingInstanceID;
		State mNeedMet;
		PlannedActionInstanceID mNeedyInstanceID;
	};
	using CausalLinks = rftl::deque<CausalLink>;
	CausalLinks causalLinks;
	CausalLink const& AddCausalLink( PlannedActionInstanceID fullfillingInstanceID, State const& needMet, PlannedActionInstanceID needyInstanceID )
	{
		CausalLink newLink;
		newLink.mFullfillingInstanceID = fullfillingInstanceID;
		newLink.mNeedMet = needMet;
		newLink.mNeedyInstanceID = needyInstanceID;
		return causalLinks.emplace_back( newLink );
	}
	CausalLinks const& GetCasualLinks()
	{
		return causalLinks;
	}

	using OrderingConstraint = rftl::pair<PlannedActionInstanceID, PlannedActionInstanceID>;
	rftl::unordered_set<OrderingConstraint, math::RawBytesHash<OrderingConstraint> > orderingConstraints;
	bool AddOrderingConstraint( PlannedActionInstanceID former, PlannedActionInstanceID latter )
	{
		orderingConstraints.emplace( former, latter );
		return true;
	}
	void AddConstraintToProtectNeedLoss( CausalLink const& causalLink, PlannedActionInstanceID potentialStompingActionInstanceID )
	{
		if( causalLink.mFullfillingInstanceID == potentialStompingActionInstanceID )
		{
			return;
		}
		if( causalLink.mNeedyInstanceID == potentialStompingActionInstanceID )
		{
			return;
		}

		ActionID const potentialStompingActionID = LookupPlannedActionByInstanceID( potentialStompingActionInstanceID );
		Action const& potentialStompingAction = LookupActionInDatabase( potentialStompingActionID );
		Postconditions const& potentialStompingPostconditions = potentialStompingAction.mPostconditions;
		for( State const& potentialStompingPostcondition : potentialStompingPostconditions )
		{
			StateID const& potentialIDMatch = potentialStompingPostcondition.first;
			StateValue const& potentialStomp = potentialStompingPostcondition.second;
			if( potentialIDMatch == causalLink.mNeedMet.first )
			{
				if( potentialStomp != causalLink.mNeedMet.second )
				{
					// Stomp!

					// Prevent with an ordering constraint

					// Add before it's fulfilled
					bool const success1 = AddOrderingConstraint( potentialStompingActionInstanceID, causalLink.mFullfillingInstanceID );
					if( success1 == false )
					{
						// Add after it's consumed
						bool const success2 = AddOrderingConstraint( causalLink.mNeedyInstanceID, potentialStompingActionInstanceID );
						if( success2 == false )
						{
							FAIL();
						}
					}
				}
			}
		}
	}

	void Run( Preconditions const& initialConditions, Postconditions const& desiredFinalConditions )
	{
		Action initialAction;
		initialAction.mDebugName = "Initial";
		initialAction.mPostconditions = initialConditions;
		AddReservedActionToDatabase( initialAction, kReservedInitialActionID );
		PlannedActionInstanceID const initialPlannedActionInstanceID = PlanActionInstance( kReservedInitialActionID );

		Action finalAction;
		finalAction.mDebugName = "Final";
		finalAction.mPreconditions = desiredFinalConditions; // Not required except for data completeness?
		AddReservedActionToDatabase( finalAction, kReservedFinalActionID );
		PlannedActionInstanceID const finalPlannedActionInstanceID = PlanActionInstance( kReservedFinalActionID );
		for( State const& condition : desiredFinalConditions )
		{
			AddNeedForUnmetPlannedAction( finalPlannedActionInstanceID, condition );
		}

		while( HasUnmetNeeds() )
		{
			// What preconditions still need to be resolved?
			UnmetPlannedActionNeed const unmetActionNeed = PopUnmetNeed();
			PlannedActionInstanceID const& needyPlannedActionInstanceID = unmetActionNeed.first;
			State const& need = unmetActionNeed.second;

			// Do we have something planned that already solves that?
			PlannedActionInstanceID fullfillingPlannedActionInstanceID = FindPlannedActionInstanceWithPostCondition( need );
			if( fullfillingPlannedActionInstanceID == kInvalidPlannedActionInstanceID )
			{
				// Do we something not yet planned that could solve that?
				ActionID const fullfillingActionID = FindActionWithPostConditionInDatabase( need );
				if( fullfillingActionID == kInvalidActionID )
				{
					// Can't possibly meet that need!
					FAIL();
				}
				fullfillingPlannedActionInstanceID = PlanActionInstance( fullfillingActionID );

				// This has to happen after start
				{
					bool const orderingViable = AddOrderingConstraint( initialPlannedActionInstanceID, fullfillingPlannedActionInstanceID );
					if( orderingViable == false )
					{
						// Ordering not achievable
						FAIL();
					}
				}

				// Make sure any ordering constraints are added to prevent this new
				//  action from stomping over other needs being met
				for( CausalLink const& causalLink : GetCasualLinks() )
				{
					AddConstraintToProtectNeedLoss( causalLink, fullfillingPlannedActionInstanceID );
				}

				// Add our newly unmet needs from this new action
				Action const& newAction = LookupActionInDatabase( fullfillingActionID );
				for( State const& condition : newAction.mPreconditions )
				{
					AddNeedForUnmetPlannedAction( fullfillingPlannedActionInstanceID, condition );
				}
			}

			// The fulfiller must happen before the needer
			{
				bool const orderingViable = AddOrderingConstraint( fullfillingPlannedActionInstanceID, needyPlannedActionInstanceID );
				if( orderingViable == false )
				{
					// Ordering not achievable
					FAIL();
				}
			}

			// Add a link to indicate the need that is being met
			CausalLink const& newCausalLink = AddCausalLink( fullfillingPlannedActionInstanceID, need, needyPlannedActionInstanceID );

			// Make sure any ordering constraints are added to prevent existing
			//  actions from stomping over this need being met
			for( PlannedActionInstances::value_type const& instancePair : GetPlannedActionInstances() )
			{
				PlannedActionInstanceID const& plannedActionInstanceID = instancePair.first;
				AddConstraintToProtectNeedLoss( newCausalLink, plannedActionInstanceID );
			}
		}
	}
};

void LogicScratch()
{
	Planner planner;

	// NOP
	planner = {};
	{
		Planner::Preconditions initialConditions;
		initialConditions.emplace( 'A', true );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}

	// Trivial single
	planner = {};
	{
		{
			Planner::Action action;
			action.mDebugName = "+A";
			action.mPostconditions.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}

	// Trivial double
	planner = {};
	{
		{
			Planner::Action action;
			action.mDebugName = "+AB";
			action.mPostconditions.emplace( 'A', true );
			action.mPostconditions.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		desiredFinalConditions.emplace( 'B', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}

	// Independent pair
	planner = {};
	{
		{
			Planner::Action action;
			action.mDebugName = "+A";
			action.mPostconditions.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mDebugName = "+B";
			action.mPostconditions.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		desiredFinalConditions.emplace( 'B', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}

	// Dependent pair
	planner = {};
	{
		{
			Planner::Action action;
			action.mDebugName = "+A";
			action.mPostconditions.emplace( 'A', true );
			action.mPostconditions.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mDebugName = "+B";
			action.mPostconditions.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		desiredFinalConditions.emplace( 'B', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}

	// Mutually dependent pair
	planner = {};
	{
		{
			Planner::Action action;
			action.mDebugName = "+A";
			action.mPostconditions.emplace( 'A', true );
			action.mPostconditions.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mDebugName = "+B";
			action.mPostconditions.emplace( 'A', false );
			action.mPostconditions.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.emplace( 'A', true );
		desiredFinalConditions.emplace( 'B', true );
		planner.Run( initialConditions, desiredFinalConditions );
	}
}

///////////////////////////////////////////////////////////////////////////////
}