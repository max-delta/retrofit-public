#pragma once

#include "core_logic/ActionDatabase.h"
#include "core_logic/DirectedEdgeGraph.h"

#include "rftl/deque"


namespace RF::logic {
///////////////////////////////////////////////////////////////////////////////

template<
	typename TMetaValue,
	typename TAlloc = rftl::allocator<uint64_t>>
struct PartialPlan
{
	using MetaValue = TMetaValue;
	using PlannedActionID = uint64_t;
	using Allocator = TAlloc;
	using AllocatorTraits = rftl::allocator_traits<Allocator>;
private:
	using ReboundPair1 = rftl::pair<PlannedActionID const, MetaValue>;
	using ReboundPair1Allocator = typename AllocatorTraits::template rebind_alloc<ReboundPair1>;
public:
	using PlannedActionIDMap = rftl::unordered_map<PlannedActionID, MetaValue, rftl::hash<PlannedActionID>, rftl::equal_to<PlannedActionID>, ReboundPair1Allocator>;
	using PlannedActionDependencyList = rftl::unordered_set<PlannedActionID, rftl::hash<PlannedActionID>, rftl::equal_to<PlannedActionID>, Allocator>;
private:
	using ReboundPair2 = rftl::pair<PlannedActionID const, PlannedActionDependencyList>;
	using ReboundPair2Allocator = typename AllocatorTraits::template rebind_alloc<ReboundPair2>;
public:
	using PlannedActionDependencyMap = rftl::unordered_map<PlannedActionID, PlannedActionDependencyList, rftl::hash<PlannedActionID>, rftl::equal_to<PlannedActionID>, ReboundPair2Allocator>;

	PlannedActionIDMap mPlannedActions;
	PlannedActionDependencyMap mDependencies;
};



template<
	typename TStateID,
	typename TStateValue,
	typename TMetaValue = EmptyStruct,
	typename THash = rftl::hash<TStateID>,
	typename TEquals = rftl::equal_to<TStateID>,
	typename TAlloc = rftl::allocator<TStateID>>
class PartialPlanner
{
	//
	// Forwards
private:
	struct CausalLink;


	//
	// Types and constants
public:
	using Hash = THash;
	using Equals = TEquals;
	using Allocator = TAlloc;
	using AllocatorTraits = rftl::allocator_traits<Allocator>;
	using ActionDatabase = logic::ActionDatabase<TStateID, TStateValue, TMetaValue, Hash, Equals, Allocator>;
	using ActionID = typename ActionDatabase::ActionID;
	using Action = typename ActionDatabase::Action;
	using Preconditions = typename Action::Preconditions;
	using Postconditions = typename Action::Postconditions;
	using State = typename Preconditions::State;
	using MetaValue = typename Action::MetaValue;
private:
	using ActionIDCollection = typename ActionDatabase::ActionIDCollection;
	using PlannedActionInstanceID = uint64_t;
	using PlannedActionInstanceMapAllocator = typename AllocatorTraits::template rebind_alloc<rftl::pair<PlannedActionInstanceID const, ActionID>>;
	using PlannedActionInstanceMap = rftl::unordered_map<PlannedActionInstanceID, ActionID, rftl::hash<PlannedActionInstanceID>, rftl::equal_to<PlannedActionInstanceID>, PlannedActionInstanceMapAllocator>;
	using PlannedActionInstanceListAllocator = typename AllocatorTraits::template rebind_alloc<PlannedActionInstanceID>;
	using PlannedActionInstanceList = rftl::deque<PlannedActionInstanceID, PlannedActionInstanceListAllocator>;
	static constexpr PlannedActionInstanceID kInvalidPlannedActionInstanceID = 0;
	static constexpr PlannedActionInstanceID kReservedInitialPlannedActionInstanceID = 1;
	static constexpr PlannedActionInstanceID kReservedFinalPlannedActionInstanceID = 2;
	static constexpr PlannedActionInstanceID kFirstGenerateablePlannedActionInstanceID = 3;
	using UnmetPlannedActionNeed = rftl::pair<PlannedActionInstanceID, State>;
	using UnmetPlannedActionNeedsAllocator = typename AllocatorTraits::template rebind_alloc<UnmetPlannedActionNeed>;
	using UnmetPlannedActionNeeds = rftl::unordered_set<UnmetPlannedActionNeed, math::RawBytesHash<UnmetPlannedActionNeed>, rftl::equal_to<UnmetPlannedActionNeed>, UnmetPlannedActionNeedsAllocator>;
	using CausalLinksAllocator = typename AllocatorTraits::template rebind_alloc<CausalLink>;
	using CausalLinks = rftl::deque<CausalLink, CausalLinksAllocator>;
	using OrderingConstraintsAllocator = typename AllocatorTraits::template rebind_alloc<PlannedActionInstanceID>;
	using OrderingConstraints = logic::DirectedEdgeGraph<PlannedActionInstanceID, EmptyStruct, rftl::hash<PlannedActionInstanceID>, rftl::equal_to<PlannedActionInstanceID>, OrderingConstraintsAllocator>;
	using PlanAllocator = typename AllocatorTraits::template rebind_alloc<uint64_t>;
public:
	using PartialPlan = PartialPlan<typename Action::MetaValue, PlanAllocator>;


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
		return mActionDatabase.AddAction( action );
	}

	bool Run( MetaValue const& initial, Preconditions const& initialConditions, MetaValue const& final, Postconditions const& desiredFinalConditions )
	{
		mPlannedActionInstances = {};
		mUnmetPlannedActionNeeds = {};
		mCausalLinks = {};
		mOrderingConstraints = {};

		Action initialAction;
		initialAction.mMeta = initial;
		initialAction.mPostconditions = initialConditions;
		ActionID const initialActionID = mActionDatabase.AddAction( initialAction );
		PlanReservedActionInstance( initialActionID, kReservedInitialPlannedActionInstanceID );

		Action finalAction;
		finalAction.mMeta = final;
		finalAction.mPreconditions = desiredFinalConditions; // Not required except for data completeness?
		ActionID const finalActionID = mActionDatabase.AddAction( finalAction );
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
				ActionIDCollection const fullfillingActionIDList = mActionDatabase.FindActionsWithPostCondition( need );
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
						RF_TODO_BREAK_MSG( "Planner unwind and try again" );
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
						RF_TODO_BREAK_MSG( "Planner unwind and try again" );
						return false;
					}
				}

				// Add our newly unmet needs from this new action
				Action const* newAction = mActionDatabase.LookupAction( fullfillingActionID );
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
					RF_TODO_BREAK_MSG( "Planner unwind and try again" );
					return false;
				}
			}

			// Add a link to indicate the need that is being met
			CausalLink const& newCausalLink = AddCausalLink( fullfillingPlannedActionInstanceID, need, needyPlannedActionInstanceID );

			// Make sure any ordering constraints are added to prevent existing
			//  actions from stomping over this need being met
			for( typename PlannedActionInstanceMap::value_type const& instancePair : GetPlannedActionInstances() )
			{
				PlannedActionInstanceID const& plannedActionInstanceID = instancePair.first;
				bool const constraintSuccess = AddConstraintToProtectNeedLoss( newCausalLink, plannedActionInstanceID );
				if( constraintSuccess == false )
				{
					RF_TODO_BREAK_MSG( "Planner unwind and try again" );
					return false;
				}
			}
		}

		return true;
	}

	PartialPlan FetchPlan() const
	{
		PartialPlan retVal;

		// Store planned actions
		for( typename PlannedActionInstanceMap::value_type const& planPair : mPlannedActionInstances )
		{
			PlannedActionInstanceID const& planID = planPair.first;
			ActionID const& actionID = planPair.second;
			Action const* action = mActionDatabase.LookupAction( actionID );
			RF_ASSERT( action != nullptr );
			typename Action::MetaValue const& meta = action->mMeta;

			retVal.mPlannedActions.emplace( planID, meta );
		}

		// Store dependencies
		typename PartialPlan::PlannedActionDependencyMap& dependencyMap = retVal.mDependencies;
		auto incomingLinkIter = [&dependencyMap](
			typename OrderingConstraints::ConstIterator const& iter ) -> bool
		{
			dependencyMap.at( iter.to ).emplace( iter.from );
			return true;
		};
		for( typename PartialPlan::PlannedActionIDMap::value_type const& planPair : retVal.mPlannedActions )
		{
			typename PartialPlan::PlannedActionID const& planID = planPair.first;
			dependencyMap[planID];
			mOrderingConstraints.IterateEdgesTo( planID, incomingLinkIter );
		}

		return retVal;
	}


	//
	// Private methods
private:
	PlannedActionInstanceID PlanActionInstance( ActionID actionID )
	{
		PlannedActionInstanceID const newPlannedActionInstanceID = mPlannedActionInstanceIDGenerator++;
		mPlannedActionInstances[newPlannedActionInstanceID] = actionID;
		return newPlannedActionInstanceID;
	}
	void PlanReservedActionInstance( ActionID actionID, PlannedActionInstanceID reservedInstanceID )
	{
		RF_ASSERT( reservedInstanceID == kReservedInitialPlannedActionInstanceID || reservedInstanceID == kReservedFinalPlannedActionInstanceID );
		mPlannedActionInstances[reservedInstanceID] = actionID;
	}
	PlannedActionInstanceMap const& GetPlannedActionInstances()
	{
		return mPlannedActionInstances;
	}
	ActionID const& LookupPlannedActionByInstanceID( PlannedActionInstanceID instanceID )
	{
		return mPlannedActionInstances.at( instanceID );
	}
	PlannedActionInstanceList FindPlannedActionInstancesWithPostCondition( State const& desiredPostCondition )
	{
		PlannedActionInstanceList retVal;
		for( typename PlannedActionInstanceMap::value_type const& actionPair : mPlannedActionInstances )
		{
			PlannedActionInstanceID const instanceID = actionPair.first;
			ActionID const actionID = actionPair.second;
			Action const* action = mActionDatabase.LookupAction( actionID );
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
		mUnmetPlannedActionNeeds.emplace( actionID, reason );
	}
	bool HasUnmetNeeds()
	{
		return mUnmetPlannedActionNeeds.empty() == false;
	}
	UnmetPlannedActionNeed PopUnmetNeed()
	{
		typename UnmetPlannedActionNeeds::const_iterator iter = mUnmetPlannedActionNeeds.begin();
		UnmetPlannedActionNeed const retVal = *iter;
		mUnmetPlannedActionNeeds.erase( iter );
		return retVal;
	}

	CausalLink const& AddCausalLink( PlannedActionInstanceID fullfillingInstanceID, State const& needMet, PlannedActionInstanceID needyInstanceID )
	{
		CausalLink newLink;
		newLink.mFullfillingInstanceID = fullfillingInstanceID;
		newLink.mNeedMet = needMet;
		newLink.mNeedyInstanceID = needyInstanceID;
		return mCausalLinks.emplace_back( newLink );
	}
	CausalLinks const& GetCasualLinks()
	{
		return mCausalLinks;
	}

	bool AddOrderingConstraint( PlannedActionInstanceID former, PlannedActionInstanceID latter )
	{
		if( mOrderingConstraints.GetEdgeIfExists( former, latter ) != nullptr )
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

		mOrderingConstraints.InsertEdge( former, latter );
		if( IsOrderingViable() )
		{
			return true;
		}
		else
		{
			// Revert
			mOrderingConstraints.EraseEdge( former, latter );
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
			typename OrderingConstraints::ConstIterator const& iter )->
			bool
		{
			edges.emplace( iter.from, iter.to );
			return true;
		};
		mOrderingConstraints.IterateEdges( acquireAllEdges );

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
			typename EdgeSet::const_iterator iter = edges.begin();
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
		Action const* potentialStompingAction = mActionDatabase.LookupAction( potentialStompingActionID );
		RF_ASSERT( potentialStompingAction != nullptr );
		Postconditions const& potentialStompingPostconditions = potentialStompingAction->mPostconditions;
		for( State const& potentialStompingPostcondition : potentialStompingPostconditions.mStates )
		{
			typename State::StateID const& potentialIDMatch = potentialStompingPostcondition.mStateID;
			typename State::StateValue const& potentialStomp = potentialStompingPostcondition.mStateValue;
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
							RF_TODO_BREAK_MSG( "Planner unwind and try again" );
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
	ActionDatabase mActionDatabase;

	PlannedActionInstanceMap mPlannedActionInstances;
	PlannedActionInstanceID mPlannedActionInstanceIDGenerator = kFirstGenerateablePlannedActionInstanceID;

	UnmetPlannedActionNeeds mUnmetPlannedActionNeeds;

	CausalLinks mCausalLinks;
	OrderingConstraints mOrderingConstraints;
};

///////////////////////////////////////////////////////////////////////////////
}
