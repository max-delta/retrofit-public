#pragma once
#include "core/macros.h"
#include "core/meta/Empty.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF { namespace logic {
///////////////////////////////////////////////////////////////////////////////

template<
	typename TNodeID,
	typename TEdgeMetaData = EmptyStruct,
	typename THash = rftl::hash<TNodeID>,
	typename TEquals = rftl::equal_to<TNodeID>,
	typename TAlloc = rftl::allocator<TNodeID>>
class DirectedEdgeGraph
{
	//
	// Types and constants
public:
	using NodeID = TNodeID;
	using EdgeMetaData = TEdgeMetaData;
	using Hash = THash;
	using Equals = TEquals;
	using Allocator = TAlloc;
	using AllocatorTraits = rftl::allocator_traits<Allocator>;
private:
	using ReboundPair1 = rftl::pair<NodeID const, EdgeMetaData>;
	using ReboundAllocator1 = typename AllocatorTraits::template rebind_alloc<ReboundPair1>;
	using EdgeTargetMap = rftl::unordered_map<NodeID, EdgeMetaData, Hash, Equals, ReboundAllocator1>;
	using ReboundPair2 = rftl::pair<NodeID const, EdgeTargetMap>;
	using ReboundAllocator2 = typename AllocatorTraits::template rebind_alloc<ReboundPair2>;
	using EdgeMap = rftl::unordered_map<NodeID, EdgeTargetMap, Hash, Equals, ReboundAllocator2>;


	//
	// Structs
public:
	struct ConstIterator
	{
		RF_NO_COPY( ConstIterator );
		ConstIterator(
			NodeID const& from,
			NodeID const& to,
			EdgeMetaData const& meta )
			: from( from )
			, to( to )
			, meta( meta )
		{
			//
		}
		NodeID const& from;
		NodeID const& to;
		EdgeMetaData const& meta;
	};
	struct Iterator
	{
		RF_NO_COPY( Iterator );
		Iterator(
			NodeID const& from,
			NodeID const& to,
			EdgeMetaData& meta )
			: from( from )
			, to( to )
			, meta( meta )
		{
			//
		}
		NodeID const& from;
		NodeID const& to;
		EdgeMetaData& meta;
	};


	//
	// Public methods
public:
	DirectedEdgeGraph() = default;

	// NOTE: Null if edge not present
	// WARNING: Invalidated on Insert/Erase
	EdgeMetaData const* GetEdgeIfExists( NodeID const& from, NodeID const& to ) const
	{
		typename EdgeMap::const_iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return nullptr;
		}
		EdgeTargetMap const& targetMap = srcIter->second;
		typename EdgeTargetMap::const_iterator const targetIter = targetMap.find( to );
		if( targetIter == targetMap.end() )
		{
			return nullptr;
		}
		EdgeMetaData const& meta = targetIter->second;
		return &meta;
	}
	EdgeMetaData* GetMutableEdgeIfExists( NodeID const& from, NodeID const& to )
	{
		EdgeMetaData const* const retVal = GetEdgeIfExists( from, to );
		return const_cast<EdgeMetaData*>( retVal );
	}

	bool Empty() const
	{
		return mEdgeMap.empty();
	}

	void InsertEdge( NodeID const& from, NodeID const& to )
	{
		InsertEdge( from, to, EdgeMetaData{} );
	}
	void InsertEdge( NodeID const& from, NodeID const& to, EdgeMetaData const& meta )
	{
		mEdgeMap[from][to] = meta;
	}

	void EraseEdge( NodeID const& from, NodeID const& to )
	{
		typename EdgeMap::iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return;
		}
		EdgeTargetMap& targetMap = srcIter->second;
		targetMap.erase( to );
		if( targetMap.empty() )
		{
			mEdgeMap.erase( srcIter );
		}
	}
	void EraseAllEdgesFrom( NodeID const& from )
	{
		mEdgeMap.erase( from );
	}
	void EraseAllEdgesTo( NodeID const& to )
	{
		typename EdgeMap::iterator srcIter = mEdgeMap.begin();
		while( srcIter != mEdgeMap.end() )
		{
			EdgeTargetMap& targetMap = srcIter->second;
			targetMap.erase( to );
			if( targetMap.empty() )
			{
				srcIter = mEdgeMap.erase( srcIter );
			}
			else
			{
				srcIter++;
			}
		}
	}
	void EraseNode( NodeID const& node )
	{
		EraseAllEdgesFrom( node );
		EraseAllEdgesTo( node );
	}

	template<typename Functor>
	void IterateEdges( Functor& functor ) const
	{
		for( typename EdgeMap::value_type const& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap const& targetMap = srcPair.second;
			for( typename EdgeTargetMap::value_type const& targetPair : targetMap )
			{
				NodeID const& target = targetPair.first;
				EdgeMetaData const& meta = targetPair.second;
				ConstIterator const iter( src, target, meta );
				bool const continueIter = functor( iter );
				if( continueIter == false )
				{
					return;
				}
			}
		}
	}
	template<typename Functor>
	void IterateMutableEdges( Functor& functor )
	{
		for( typename EdgeMap::value_type& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap& targetMap = srcPair.second;
			for( typename EdgeTargetMap::value_type& targetPair : targetMap )
			{
				NodeID const& target = targetPair.first;
				EdgeMetaData& meta = targetPair.second;
				Iterator const iter( src, target, meta );
				bool const continueIter = functor( iter );
				if( continueIter == false )
				{
					return;
				}
			}
		}
	}
	template<typename Functor>
	void IterateEdgesFrom( NodeID const& from, Functor& functor ) const
	{
		typename EdgeMap::const_iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return;
		}
		NodeID const& src = srcIter->first;
		EdgeTargetMap const& targetMap = srcIter->second;
		for( typename EdgeTargetMap::value_type const& targetPair : targetMap )
		{
			NodeID const& target = targetPair.first;
			EdgeMetaData const& meta = targetPair.second;
			ConstIterator const iter( src, target, meta );
			bool const continueIter = functor( iter );
			if( continueIter == false )
			{
				return;
			}
		}
	}
	template<typename Functor>
	void IterateMutableEdgesFrom( NodeID const& from, Functor& functor )
	{
		typename EdgeMap::iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return;
		}
		NodeID const& src = srcIter->first;
		EdgeTargetMap& targetMap = srcIter->second;
		for( typename EdgeTargetMap::value_type& targetPair : targetMap )
		{
			NodeID const& target = targetPair.first;
			EdgeMetaData& meta = targetPair.second;
			Iterator const iter( src, target, meta );
			bool const continueIter = functor( iter );
			if( continueIter == false )
			{
				return;
			}
		}
	}
	template<typename Functor>
	void IterateEdgesTo( NodeID const& to, Functor& functor ) const
	{
		for( typename EdgeMap::value_type const& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap const& targetMap = srcPair.second;
			typename EdgeTargetMap::const_iterator const targetIter = targetMap.find( to );
			if( targetIter == targetMap.end() )
			{
				continue;
			}
			NodeID const& target = targetIter->first;
			EdgeMetaData const& meta = targetIter->second;
			ConstIterator const iter( src, target, meta );
			bool const continueIter = functor( iter );
			if( continueIter == false )
			{
				return;
			}
		}
	}
	template<typename Functor>
	void IterateMutableEdgesTo( NodeID const& to, Functor& functor )
	{
		for( typename EdgeMap::value_type& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap& targetMap = srcPair.second;
			typename EdgeTargetMap::iterator const targetIter = targetMap.find( to );
			if( targetIter == targetMap.end() )
			{
				continue;
			}
			NodeID const& target = targetIter->first;
			EdgeMetaData& meta = targetIter->second;
			Iterator const iter( src, target, meta );
			bool const continueIter = functor( iter );
			if( continueIter == false )
			{
				return;
			}
		}
	}


	//
	// Private data
private:
	EdgeMap mEdgeMap;
};

///////////////////////////////////////////////////////////////////////////////
}}
