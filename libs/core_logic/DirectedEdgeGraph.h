#pragma once
#include "core/macros.h"
#include "core/meta/Empty.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF { namespace logic {
///////////////////////////////////////////////////////////////////////////////

class DirectedEdgeGraph
{
	RF_NO_COPY( DirectedEdgeGraph );

	//
	// Types and constants
public:
	using EdgeMetaData = EmptyStruct; // TODO: Templatize
	using NodeID = int; // TODO: Templatize
	using Hash = rftl::hash<NodeID>; // TODO: Templatize
	using Equals = rftl::equal_to<NodeID>; // TODO: Templatize
	using Allocator = rftl::allocator<NodeID>; // TODO: Templatize
private:
	using ReboundAllocator1 = rftl::allocator_traits<Allocator>::rebind_alloc< rftl::pair<NodeID const, EdgeMetaData> >;
	using EdgeTargetMap = rftl::unordered_map<NodeID, EdgeMetaData, Hash, Equals, ReboundAllocator1>;
	using ReboundAllocator2 = rftl::allocator_traits<Allocator>::rebind_alloc< rftl::pair<NodeID const, EdgeTargetMap> >;
	using EdgeMap = rftl::unordered_map<NodeID, EdgeTargetMap, Hash, Equals, ReboundAllocator2>;


	//
	// Structs
public:
	struct ConstIterator
	{
		RF_NO_COPY( ConstIterator );
		NodeID const& from;
		NodeID const& to;
		EdgeMetaData const& meta;
	};
	struct Iterator
	{
		RF_NO_COPY( Iterator );
		NodeID const& from;
		NodeID const& to;
		EdgeMetaData& meta;
	};


	//
	// Public methods
public:
	DirectedEdgeGraph() = default;
	DirectedEdgeGraph( DirectedEdgeGraph&& ) = default;
	DirectedEdgeGraph& operator=( DirectedEdgeGraph&& ) = default;

	// NOTE: Null if edge not present
	// WARNING: Invalidated on Insert/Erase
	EdgeMetaData const* GetEdgeIfExists( NodeID const& from, NodeID const& to ) const
	{
		EdgeMap::const_iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return nullptr;
		}
		EdgeTargetMap const& targetMap = srcIter->second;
		EdgeTargetMap::const_iterator const targetIter = targetMap.find( to );
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
		EdgeMap::iterator const srcIter = mEdgeMap.find( from );
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
		EdgeMap::iterator srcIter = mEdgeMap.begin();
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
		for( EdgeMap::value_type const& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap const& targetMap = srcPair.second;
			for( EdgeTargetMap::value_type const& targetPair : targetMap )
			{
				NodeID const& target = targetPair.first;
				EdgeMetaData const& meta = targetPair.second;
				ConstIterator const iter{ src, target, meta };
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
		for( EdgeMap::value_type& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap& targetMap = srcPair.second;
			for( EdgeTargetMap::value_type& targetPair : targetMap )
			{
				NodeID const& target = targetPair.first;
				EdgeMetaData& meta = targetPair.second;
				Iterator const iter{ src, target, meta };
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
		EdgeMap::const_iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return;
		}
		NodeID const& src = srcIter->first;
		EdgeTargetMap const& targetMap = srcIter->second;
		for( EdgeTargetMap::value_type const& targetPair : targetMap )
		{
			NodeID const& target = targetPair.first;
			EdgeMetaData const& meta = targetPair.second;
			ConstIterator const iter{ src, target, meta };
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
		EdgeMap::iterator const srcIter = mEdgeMap.find( from );
		if( srcIter == mEdgeMap.end() )
		{
			return;
		}
		NodeID const& src = srcIter->first;
		EdgeTargetMap& targetMap = srcIter->second;
		for( EdgeTargetMap::value_type& targetPair : targetMap )
		{
			NodeID const& target = targetPair.first;
			EdgeMetaData& meta = targetPair.second;
			Iterator const iter{ src, target, meta };
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
		for( EdgeMap::value_type const& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap const& targetMap = srcPair.second;
			EdgeTargetMap::const_iterator const targetIter = targetMap.find( to );
			if( targetIter == targetMap.end() )
			{
				continue;
			}
			NodeID const& target = targetIter->first;
			EdgeMetaData const& meta = targetIter->second;
			ConstIterator const iter{ src, target, meta };
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
		for( EdgeMap::value_type& srcPair : mEdgeMap )
		{
			NodeID const& src = srcPair.first;
			EdgeTargetMap& targetMap = srcPair.second;
			EdgeTargetMap::iterator const targetIter = targetMap.find( to );
			if( targetIter == targetMap.end() )
			{
				continue;
			}
			NodeID const& target = targetIter->first;
			EdgeMetaData& meta = targetIter->second;
			Iterator const iter{ src, target, meta };
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
