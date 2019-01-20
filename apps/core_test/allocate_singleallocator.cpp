#include "stdafx.h"

#include "core_allocate/SingleAllocator.h"

#include "rftl/utility"
//#include "rftl/vector"
//#include "rftl/deque"
//#include "rftl/list"
//#include "rftl/set"
//#include "rftl/map"
//#include "rftl/unordered_set"
//#include "rftl/unordered_map"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( SingleAllocator, Standalone )
{
	SingleAllocator<uint64_t, 8> alloc{ ExplicitDefaultConstruct() };
	for( uint64_t i = 0; i < 4; i++ )
	{
		uint64_t* const allocation = alloc.allocate( 1 );
		ASSERT_NE( allocation, nullptr );
		alloc.construct( allocation, i );
		ASSERT_EQ( *allocation, i );
		alloc.destroy( allocation );
		alloc.deallocate( allocation, 1 );
	}
}

TEST( SingleAllocator, STLContainers )
{
	using IntAllocator = SingleAllocator<int, 1024>;
	using KVAllocator = SingleAllocator<rftl::pair<int const, int>, 1024>;
	IntAllocator alloc_i{ ExplicitDefaultConstruct() };
	KVAllocator alloc_kv{ ExplicitDefaultConstruct() };

	// Can't use, tries to copy and mutate
	//rftl::vector<int, IntAllocator> vector{ alloc_i };
	//rftl::deque<int, IntAllocator> deque{ alloc_i };
	//rftl::list<int, IntAllocator> list{ alloc_i };
	//rftl::set<int, rftl::less<int>, IntAllocator> set{ alloc_i };
	//rftl::map<int, int, rftl::less<int>, KVAllocator> set{ alloc_kv };
	//rftl::unordered_set<int, rftl::hash<int>, rftl::equal_to<int>, IntAllocator> set{ alloc_i };
	//rftl::unordered_map<int, int, rftl::hash<int>, rftl::equal_to<int>, KVAllocator> set{ alloc_kv };
}

///////////////////////////////////////////////////////////////////////////////
}
