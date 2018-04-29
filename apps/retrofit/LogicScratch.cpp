#include "stdafx.h"

#include "core_logic/DirectedEdgeGraph.h"
#include "core_logic/ActionDatabase.h"
#include "core_math/Hash.h"
#include "core/macros.h"

#include "rftl/unordered_set"
#include "rftl/unordered_map"
#include "rftl/deque"
#include "rftl/string"
#include "rftl/limits"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

class Planner
{
	//
	// Forwards
private:
	struct CausalLink;


	//
	// Types and constants
public:
	using ActionDatabase = logic::ActionDatabase<char, bool, rftl::string>;
	using ActionID = ActionDatabase::ActionID;
	using Action = ActionDatabase::Action;
	using Preconditions = Action::Preconditions;
	using Postconditions = Action::Postconditions;
	using State = Preconditions::State;
private:
	using ActionIDCollection = ActionDatabase::ActionIDCollection;
	using PlannedActionInstanceID = uint64_t;
	using PlannedActionInstanceMap = rftl::unordered_map<PlannedActionInstanceID, ActionID>;
	using PlannedActionInstanceList = rftl::deque<PlannedActionInstanceID>;
	static constexpr PlannedActionInstanceID kInvalidPlannedActionInstanceID = 0;
	static constexpr PlannedActionInstanceID kReservedInitialPlannedActionInstanceID = 1;
	static constexpr PlannedActionInstanceID kReservedFinalPlannedActionInstanceID = 2;
	static constexpr PlannedActionInstanceID kFirstGenerateablePlannedActionInstanceID = 3;
	using UnmetPlannedActionNeed = rftl::pair<PlannedActionInstanceID, State>;
	using UnmetPlannedActionNeeds = rftl::unordered_set<UnmetPlannedActionNeed, math::RawBytesHash<UnmetPlannedActionNeed> >;
	using CausalLinks = rftl::deque<CausalLink>;
	using OrderingConstraints = logic::DirectedEdgeGraph<PlannedActionInstanceID>;


	//
	// Structs
private:
	struct CausalLink
	{
		PlannedActionInstanceID mFullfillingInstanceID;
		State mNeedMet;
		PlannedActionInstanceID mNeedyInstanceID;
	};


	//
	// Public methods
public:
	ActionID AddActionToDatabase( Action const& action )
	{
		return actionDatabase.AddAction( action );
	}

	bool Run( Preconditions const& initialConditions, Postconditions const& desiredFinalConditions )
	{
		Action initialAction;
		initialAction.mMeta = "Initial";
		initialAction.mPostconditions = initialConditions;
		ActionID const initialActionID = actionDatabase.AddAction( initialAction );
		PlanReservedActionInstance( initialActionID, kReservedInitialPlannedActionInstanceID );

		Action finalAction;
		finalAction.mMeta = "Final";
		finalAction.mPreconditions = desiredFinalConditions; // Not required except for data completeness?
		ActionID const finalActionID = actionDatabase.AddAction( finalAction );
		PlanReservedActionInstance( finalActionID, kReservedFinalPlannedActionInstanceID );
		for( State const& condition : desiredFinalConditions.mStates )
		{
			AddNeedForUnmetPlannedAction( kReservedFinalPlannedActionInstanceID, condition );
		}

		while( HasUnmetNeeds() )
		{
			// What preconditions still need to be resolved?
			UnmetPlannedActionNeed const unmetActionNeed = PopUnmetNeed();
			PlannedActionInstanceID const& needyPlannedActionInstanceID = unmetActionNeed.first;
			State const& need = unmetActionNeed.second;

			// Do we have something planned that already solves that?
			PlannedActionInstanceList const fullfillingPlannedActionInstanceIDs = FindPlannedActionInstancesWithPostCondition( need );
			PlannedActionInstanceID fullfillingPlannedActionInstanceID = kInvalidPlannedActionInstanceID;
			if( fullfillingPlannedActionInstanceIDs.empty() == false )
			{
				// HACK: Choose just one, but it might not work
				RF_ASSERT_MSG(
					fullfillingPlannedActionInstanceIDs.size() == 1,
					"Planner found multiple ways to solve precondition. Choice resolution not yet implemented." );
				fullfillingPlannedActionInstanceID = fullfillingPlannedActionInstanceIDs.front();
			}
			if( fullfillingPlannedActionInstanceID == kInvalidPlannedActionInstanceID )
			{
				// Do we have something not yet planned that could solve that?
				ActionIDCollection const fullfillingActionIDList = actionDatabase.FindActionsWithPostCondition( need );
				ActionID fullfillingActionID = ActionDatabase::kInvalidActionID;
				if( fullfillingActionIDList.empty() == false )
				{
					// HACK: Choose just one, but it might not work
					RF_ASSERT_MSG(
						fullfillingActionIDList.size() == 1,
						"Planner found multiple ways to solve precondition. Choice resolution not yet implemented." );
					fullfillingActionID = *fullfillingActionIDList.begin();
				}
				if( fullfillingActionID == ActionDatabase::kInvalidActionID )
				{
					// Can't possibly meet that need!
					RF_DBGFAIL_MSG( "Planner can't find action in database to fulfill post-condition" );
					return false;
				}
				fullfillingPlannedActionInstanceID = PlanActionInstance( fullfillingActionID );

				// This has to happen after start
				{
					bool const orderingViable = AddOrderingConstraint( kReservedInitialPlannedActionInstanceID, fullfillingPlannedActionInstanceID );
					if( orderingViable == false )
					{
						// Ordering not achievable
						RF_DBGFAIL_MSG( "TODO: Planner unwind and try again" );
						return false;
					}
				}

				// Make sure any ordering constraints are added to prevent this new
				//  action from stomping over other needs being met
				for( CausalLink const& causalLink : GetCasualLinks() )
				{
					bool const constraintSuccess = AddConstraintToProtectNeedLoss( causalLink, fullfillingPlannedActionInstanceID );
					if( constraintSuccess == false )
					{
						RF_DBGFAIL_MSG( "TODO: Planner unwind and try again" );
						return false;
					}
				}

				// Add our newly unmet needs from this new action
				Action const* newAction = actionDatabase.LookupAction( fullfillingActionID );
				RF_ASSERT( newAction != nullptr );
				for( State const& condition : newAction->mPreconditions.mStates )
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
					RF_DBGFAIL_MSG( "TODO: Planner unwind and try again" );
					return false;
				}
			}

			// Add a link to indicate the need that is being met
			CausalLink const& newCausalLink = AddCausalLink( fullfillingPlannedActionInstanceID, need, needyPlannedActionInstanceID );

			// Make sure any ordering constraints are added to prevent existing
			//  actions from stomping over this need being met
			for( PlannedActionInstanceMap::value_type const& instancePair : GetPlannedActionInstances() )
			{
				PlannedActionInstanceID const& plannedActionInstanceID = instancePair.first;
				bool const constraintSuccess = AddConstraintToProtectNeedLoss( newCausalLink, plannedActionInstanceID );
				if( constraintSuccess == false )
				{
					RF_DBGFAIL_MSG( "TODO: Planner unwind and try again" );
					return false;
				}
			}
		}

		return true;
	}


	//
	// Private methods
private:
	PlannedActionInstanceID PlanActionInstance( ActionID actionID )
	{
		PlannedActionInstanceID const newPlannedActionInstanceID = plannedActionInstanceIDGenerator++;
		plannedActionInstances[newPlannedActionInstanceID] = actionID;
		return newPlannedActionInstanceID;
	}
	void PlanReservedActionInstance( ActionID actionID, PlannedActionInstanceID reservedInstanceID )
	{
		RF_ASSERT( reservedInstanceID == kReservedInitialPlannedActionInstanceID || reservedInstanceID == kReservedFinalPlannedActionInstanceID );
		plannedActionInstances[reservedInstanceID] = actionID;
	}
	PlannedActionInstanceMap const& GetPlannedActionInstances()
	{
		return plannedActionInstances;
	}
	ActionID const& LookupPlannedActionByInstanceID( PlannedActionInstanceID instanceID )
	{
		return plannedActionInstances.at( instanceID );
	}
	PlannedActionInstanceList FindPlannedActionInstancesWithPostCondition( State const& desiredPostCondition )
	{
		PlannedActionInstanceList retVal;
		for( PlannedActionInstanceMap::value_type const& actionPair : plannedActionInstances )
		{
			PlannedActionInstanceID const instanceID = actionPair.first;
			ActionID const actionID = actionPair.second;
			Action const* action = actionDatabase.LookupAction( actionID );
			RF_ASSERT( action != nullptr );
			Postconditions const& postConditions = action->mPostconditions;
			for( State const& condition : postConditions.mStates )
			{
				if( condition == desiredPostCondition )
				{
					retVal.emplace_back( instanceID );
					break;
				}
			}
		}

		return retVal;
	}

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

	bool AddOrderingConstraint( PlannedActionInstanceID former, PlannedActionInstanceID latter )
	{
		if( orderingConstraints.GetEdgeIfExists( former, latter ) != nullptr )
		{
			// Redundant constraint
			return true;
		}

		if( latter == kReservedInitialPlannedActionInstanceID )
		{
			// Can't be before start
			return false;
		}
		if( former == kReservedFinalPlannedActionInstanceID )
		{
			// Can't be after end
			return false;
		}

		orderingConstraints.InsertEdge( former, latter );
		if( IsOrderingViable() )
		{
			return true;
		}
		else
		{
			// Revert
			orderingConstraints.EraseEdge( former, latter );
			return false;
		}
	}
	bool IsOrderingViable()
	{
		using Node = PlannedActionInstanceID;
		using NodeSet = rftl::unordered_set<Node>;
		using Edge = rftl::pair<Node, Node>;
		using EdgeSet = rftl::unordered_set<Edge, math::RawBytesHash<Edge>>;
		// Topological sort of graph where edges are from former->latter
		// See: Kahn's algorithm
		EdgeSet edges = {};
		auto acquireAllEdges = [&](
			OrderingConstraints::ConstIterator const& iter )->
			bool
		{
			edges.emplace( iter.from, iter.to );
			return true;
		};
		orderingConstraints.IterateEdges( acquireAllEdges );

		NodeSet allRootIDs;
		for( Edge const& edge : edges )
		{
			allRootIDs.emplace( edge.first );
		}
		for( Edge const& edge : edges )
		{
			allRootIDs.erase( edge.second );
		}
		if( edges.empty() == false && allRootIDs.empty() )
		{
			// Trivial fail, edges without roots
			return false;
		}
		while( allRootIDs.empty() == false )
		{
			Node const n = *allRootIDs.begin();
			allRootIDs.erase( n );
			// If this was an actual sort, we'd write 'n' to tail of output now
			NodeSet affectedNodes;
			EdgeSet::const_iterator iter = edges.begin();
			while( iter != edges.end() )
			{
				if( iter->first != n )
				{
					iter++;
					continue;
				}
				Node const m = iter->second;
				iter = edges.erase( iter );
				affectedNodes.emplace( m );
			}
			for( Node const& m : affectedNodes )
			{
				bool isNowRoot = true;
				for( Edge const& edge : edges )
				{
					if( edge.second == m )
					{
						isNowRoot = false;
					}
				}
				if( isNowRoot )
				{
					allRootIDs.emplace( m );
				}
			}
		}

		if( edges.empty() )
		{
			// Was able to recursively remove all edges from all suspected
			//  roots with no edges remaining, so there was no backwards
			//  root we missed
			return true;
		}
		return false;
	}
	bool AddConstraintToProtectNeedLoss( CausalLink const& causalLink, PlannedActionInstanceID potentialStompingActionInstanceID )
	{
		if( causalLink.mFullfillingInstanceID == potentialStompingActionInstanceID )
		{
			return true;
		}
		if( causalLink.mNeedyInstanceID == potentialStompingActionInstanceID )
		{
			return true;
		}

		ActionID const potentialStompingActionID = LookupPlannedActionByInstanceID( potentialStompingActionInstanceID );
		Action const* potentialStompingAction = actionDatabase.LookupAction( potentialStompingActionID );
		RF_ASSERT( potentialStompingAction != nullptr );
		Postconditions const& potentialStompingPostconditions = potentialStompingAction->mPostconditions;
		for( State const& potentialStompingPostcondition : potentialStompingPostconditions.mStates )
		{
			State::StateID const& potentialIDMatch = potentialStompingPostcondition.mStateID;
			State::StateValue const& potentialStomp = potentialStompingPostcondition.mStateValue;
			if( potentialIDMatch == causalLink.mNeedMet.mStateID )
			{
				if( potentialStomp != causalLink.mNeedMet.mStateValue )
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
							RF_DBGFAIL_MSG( "TODO: Planner unwind and try again" );
							return false;
						}
					}
				}
			}
		}
		return true;
	}


	//
	// Private data
private:
	ActionDatabase actionDatabase;

	PlannedActionInstanceMap plannedActionInstances;
	PlannedActionInstanceID plannedActionInstanceIDGenerator = kFirstGenerateablePlannedActionInstanceID;

	UnmetPlannedActionNeeds unmetPlannedActionNeeds;

	CausalLinks causalLinks;
	OrderingConstraints orderingConstraints;
};



void LogicScratch()
{
	constexpr bool kIncludeFailureCases = false;
	constexpr bool kIncludeUnimplementedCases = false;
	Planner planner;

	// NOP
	planner = {};
	{
		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', true );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}

	// Trivial single
	planner = {};
	{
		{
			Planner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}

	// Trivial double
	planner = {};
	{
		{
			Planner::Action action;
			action.mMeta = "+AB";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}

	// Independent pair
	planner = {};
	{
		{
			Planner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}

	// Dependent pair
	planner = {};
	{
		{
			Planner::Action action;
			action.mMeta = "+A-B";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}

	// Mutually dependent pair
	planner = {};
	if( kIncludeFailureCases )
	{
		{
			Planner::Action action;
			action.mMeta = "+A-B";
			action.mPostconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'B', false );
			planner.AddActionToDatabase( action );
		}
		{
			Planner::Action action;
			action.mMeta = "+B-A";
			action.mPostconditions.mStates.emplace( 'A', false );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		Planner::Postconditions desiredFinalConditions;
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
			Planner::Action action;
			action.mMeta = "+A";
			action.mPostconditions.mStates.emplace( 'A', true );
			planner.AddActionToDatabase( action );
		}
		{
			// If the planner tries this one it will fail
			Planner::Action action;
			action.mMeta = "+B-A";
			action.mPreconditions.mStates.emplace( 'A', true );
			action.mPostconditions.mStates.emplace( 'A', false );
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}
		{
			// If the planner tries this one it will succeed
			Planner::Action action;
			action.mMeta = "+B";
			action.mPostconditions.mStates.emplace( 'B', true );
			planner.AddActionToDatabase( action );
		}

		Planner::Preconditions initialConditions;
		initialConditions.mStates.emplace( 'A', false );
		initialConditions.mStates.emplace( 'B', false );
		Planner::Postconditions desiredFinalConditions;
		desiredFinalConditions.mStates.emplace( 'A', true );
		desiredFinalConditions.mStates.emplace( 'B', true );
		bool const planSuccess = planner.Run( initialConditions, desiredFinalConditions );
		RF_ASSERT( planSuccess );
	}
}

///////////////////////////////////////////////////////////////////////////////
}