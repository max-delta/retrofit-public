#include "stdafx.h"

#include "core_allocate/RftlAllocator.h"
#include "core_allocate/LinearAllocator.h"

#include "rftl/functional"
#include "rftl/vector"
#include "rftl/deque"
//#include "rftl/list"
#include "rftl/set"
#include "rftl/map"
#include "rftl/unordered_set"
#include "rftl/unordered_map"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( RftlAllocator, RftlContainers )
{
	using IntAllocator = RftlAllocator<int>;
	using KVAllocator = RftlAllocator<rftl::pair<int const, int>>;

	AllocatorT<LinearAllocator<1024>> stackAlloc{ ExplicitDefaultConstruct() };

	IntAllocator alloc_i( stackAlloc );
	KVAllocator alloc_kv{ stackAlloc };

	{
		rftl::vector<int, IntAllocator> vector{ alloc_i };
	}
	{
		rftl::deque<int, IntAllocator> deque{ alloc_i };
	}
	// TODO: List support in rftl?
	//{
	//	rftl::list<int, IntAllocator> list{ alloc_i };
	//}
	{
		rftl::set<int, rftl::less<int>, IntAllocator> set{ alloc_i };
	}
	{
		rftl::map<int, int, rftl::less<int>, KVAllocator> map{ alloc_kv };
	}
	{
		rftl::unordered_set<int, rftl::hash<int>, rftl::equal_to<int>, IntAllocator> unordered_set{ alloc_i };
	}
	{
		rftl::unordered_map<int, int, rftl::hash<int>, rftl::equal_to<int>, KVAllocator> unordered_map{ alloc_kv };
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
