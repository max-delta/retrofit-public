#include "stdafx.h"

#include "core_logic/DirectedEdgeGraph.h"
#include "core_math/Hash.h"

#include "rftl/vector"
#include "rftl/set"
#include "rftl/tuple"


namespace RF { namespace logic {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using StandardTestGraph = DirectedEdgeGraph<int, char>;
StandardTestGraph CreateStandardTestGraph()
{
	// 0 -> 1
	// ^   /
	// |  /
	// V V
	//  2
	StandardTestGraph retVal = {};
	retVal.InsertEdge( 0, 1, 'A' );
	retVal.InsertEdge( 0, 2, 'B' );
	retVal.InsertEdge( 1, 2 );
	retVal.InsertEdge( 2, 0 );
	return retVal;
}

}



TEST( LogicDirectedEdgeGraph, Empty )
{
	using Graph = DirectedEdgeGraph<int>;
	Graph graph = {};

	ASSERT_TRUE( graph.Empty() );

	Graph::EdgeMetaData const* const e01c = graph.GetEdgeIfExists( 0, 1 );
	Graph::EdgeMetaData* const e01m = graph.GetMutableEdgeIfExists( 0, 1 );
	ASSERT_EQ( e01c, nullptr );
	ASSERT_EQ( e01m, nullptr );

	bool iterated = false;
	auto citer = [&iterated](
		Graph::ConstIterator const& )->
		bool
	{
		iterated = true;
		return true;
	};
	auto iter = [&iterated](
		Graph::Iterator const& )->
		bool
	{
		iterated = true;
		return true;
	};

	graph.IterateEdges( citer );
	ASSERT_FALSE( iterated );
	graph.IterateMutableEdges( iter );
	ASSERT_FALSE( iterated );
	graph.IterateEdgesFrom( 0, citer );
	ASSERT_FALSE( iterated );
	graph.IterateMutableEdgesFrom( 0, iter );
	ASSERT_FALSE( iterated );
	graph.IterateEdgesTo( 0, citer );
	ASSERT_FALSE( iterated );
	graph.IterateMutableEdgesTo( 0, iter );
	ASSERT_FALSE( iterated );
}



TEST( LogicDirectedEdgeGraph, CreateStandard )
{
	using Graph = details::StandardTestGraph;
	Graph graph = details::CreateStandardTestGraph();

	ASSERT_FALSE( graph.Empty() );

	Graph::EdgeMetaData const* const e01c = graph.GetEdgeIfExists( 0, 1 );
	Graph::EdgeMetaData* const e01m = graph.GetMutableEdgeIfExists( 0, 1 );
	Graph::EdgeMetaData const* const e02c = graph.GetEdgeIfExists( 0, 2 );
	Graph::EdgeMetaData* const e02m = graph.GetMutableEdgeIfExists( 0, 2 );
	Graph::EdgeMetaData const* const e10c = graph.GetEdgeIfExists( 1, 0 );
	Graph::EdgeMetaData* const e10m = graph.GetMutableEdgeIfExists( 1, 0 );
	Graph::EdgeMetaData const* const e12c = graph.GetEdgeIfExists( 1, 2 );
	Graph::EdgeMetaData* const e12m = graph.GetMutableEdgeIfExists( 1, 2 );
	Graph::EdgeMetaData const* const e20c = graph.GetEdgeIfExists( 2, 0 );
	Graph::EdgeMetaData* const e20m = graph.GetMutableEdgeIfExists( 2, 0 );
	Graph::EdgeMetaData const* const e21c = graph.GetEdgeIfExists( 2, 1 );
	Graph::EdgeMetaData* const e21m = graph.GetMutableEdgeIfExists( 2, 1 );
	ASSERT_EQ( e01c, e01m );
	ASSERT_EQ( e02c, e02m );
	ASSERT_EQ( e10c, e10m );
	ASSERT_EQ( e12c, e12m );
	ASSERT_EQ( e20c, e20m );
	ASSERT_EQ( e21c, e21m );
	ASSERT_NE( e01c, nullptr );
	ASSERT_NE( e02c, nullptr );
	ASSERT_EQ( e10c, nullptr );
	ASSERT_NE( e12c, nullptr );
	ASSERT_NE( e20c, nullptr );
	ASSERT_EQ( e21c, nullptr );
	ASSERT_EQ( *e01c, 'A' );
	ASSERT_EQ( *e02c, 'B' );

	using ResultPair = rftl::tuple<Graph::NodeID, Graph::NodeID, Graph::EdgeMetaData>;
	using ImmediateResults = rftl::vector<ResultPair>;
	ImmediateResults iterationResults;
	auto citer = [&iterationResults](
		Graph::ConstIterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};
	auto iter = [&iterationResults](
		Graph::Iterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};

	using StoreResults = rftl::set<ResultPair>;
	StoreResults iterTemp = {};

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterAllC{ rftl::move( iterTemp ) };
	StoreResults const expectAllC{ { 0, 1, 'A' }, { 0, 2, 'B' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterAllC, expectAllC );


	graph.IterateMutableEdges( iter );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterAllM{ rftl::move( iterTemp ) };
	ASSERT_EQ( iterAllM, iterAllC );

	graph.IterateEdgesFrom( 0, citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFrom0C{ rftl::move( iterTemp ) };
	StoreResults const expectFrom0C{ { 0, 1, 'A' }, { 0, 2, 'B' } };
	ASSERT_EQ( iterFrom0C, expectFrom0C );

	graph.IterateMutableEdgesFrom( 0, iter );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFrom0M{ rftl::move( iterTemp ) };
	ASSERT_EQ( iterFrom0M, iterFrom0C );

	graph.IterateEdgesTo( 2, citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterTo2C{ rftl::move( iterTemp ) };
	StoreResults const expectTo2C{ { 0, 2, 'B' }, { 1, 2, '\0' } };
	ASSERT_EQ( iterTo2C, expectTo2C );

	graph.IterateMutableEdgesTo( 2, iter );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterTo2M{ rftl::move( iterTemp ) };
	ASSERT_EQ( iterTo2M, iterTo2C );
}



TEST( LogicDirectedEdgeGraph, EraseEdge )
{
	using Graph = details::StandardTestGraph;
	Graph graph = details::CreateStandardTestGraph();

	using ResultPair = rftl::tuple<Graph::NodeID, Graph::NodeID, Graph::EdgeMetaData>;
	using ImmediateResults = rftl::vector<ResultPair>;
	ImmediateResults iterationResults;
	auto citer = [&iterationResults](
		Graph::ConstIterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};

	using StoreResults = rftl::set<ResultPair>;
	StoreResults iterTemp = {};

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterInitial{ rftl::move( iterTemp ) };
	StoreResults const expectInitial{ { 0, 1, 'A' }, { 0, 2, 'B' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterInitial, expectInitial );

	graph.EraseEdge( 0, 2 );

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFinal{ rftl::move( iterTemp ) };
	StoreResults const expectFinal{ { 0, 1, 'A' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterFinal, expectFinal );
}



TEST( LogicDirectedEdgeGraph, EraseAllEdgesFrom )
{
	using Graph = details::StandardTestGraph;
	Graph graph = details::CreateStandardTestGraph();

	using ResultPair = rftl::tuple<Graph::NodeID, Graph::NodeID, Graph::EdgeMetaData>;
	using ImmediateResults = rftl::vector<ResultPair>;
	ImmediateResults iterationResults;
	auto citer = [&iterationResults](
		Graph::ConstIterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};

	using StoreResults = rftl::set<ResultPair>;
	StoreResults iterTemp = {};

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterInitial{ rftl::move( iterTemp ) };
	StoreResults const expectInitial{ { 0, 1, 'A' }, { 0, 2, 'B' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterInitial, expectInitial );

	graph.EraseAllEdgesFrom( 0 );

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFinal{ rftl::move( iterTemp ) };
	StoreResults const expectFinal{ { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterFinal, expectFinal );
}



TEST( LogicDirectedEdgeGraph, EraseAllEdgesTo )
{
	using Graph = details::StandardTestGraph;
	Graph graph = details::CreateStandardTestGraph();

	using ResultPair = rftl::tuple<Graph::NodeID, Graph::NodeID, Graph::EdgeMetaData>;
	using ImmediateResults = rftl::vector<ResultPair>;
	ImmediateResults iterationResults;
	auto citer = [&iterationResults](
		Graph::ConstIterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};

	using StoreResults = rftl::set<ResultPair>;
	StoreResults iterTemp = {};

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterInitial{ rftl::move( iterTemp ) };
	StoreResults const expectInitial{ { 0, 1, 'A' }, { 0, 2, 'B' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterInitial, expectInitial );

	graph.EraseAllEdgesTo( 2 );

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFinal{ rftl::move( iterTemp ) };
	StoreResults const expectFinal{ { 0, 1, 'A' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterFinal, expectFinal );
}



TEST( LogicDirectedEdgeGraph, EraseNode )
{
	using Graph = details::StandardTestGraph;
	Graph graph = details::CreateStandardTestGraph();

	using ResultPair = rftl::tuple<Graph::NodeID, Graph::NodeID, Graph::EdgeMetaData>;
	using ImmediateResults = rftl::vector<ResultPair>;
	ImmediateResults iterationResults;
	auto citer = [&iterationResults](
		Graph::ConstIterator const& iter )->
		bool
	{
		iterationResults.emplace_back( iter.from, iter.to, iter.meta );
		return true;
	};

	using StoreResults = rftl::set<ResultPair>;
	StoreResults iterTemp = {};

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterInitial{ rftl::move( iterTemp ) };
	StoreResults const expectInitial{ { 0, 1, 'A' }, { 0, 2, 'B' }, { 1, 2, '\0' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterInitial, expectInitial );

	graph.EraseNode( 1 );

	graph.IterateEdges( citer );
	iterTemp = StoreResults( iterationResults.begin(), iterationResults.end() );
	ASSERT_EQ( iterTemp.size(), iterationResults.size() );
	iterationResults.clear();
	StoreResults const iterFinal{ rftl::move( iterTemp ) };
	StoreResults const expectFinal{ { 0, 2, 'B' }, { 2, 0, '\0' } };
	ASSERT_EQ( iterFinal, expectFinal );
}

///////////////////////////////////////////////////////////////////////////////
}}
