#include "stdafx.h"

#include "core_allocate/SingleAllocator.h"

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>


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

TEST(SingleAllocator, STLContainers)
{
	using IntAllocator = SingleAllocator<int, 1024>;
	using KVAllocator = SingleAllocator< std::pair<int const, int>, 1024>;
	IntAllocator alloc_i{ ExplicitDefaultConstruct() };
	KVAllocator alloc_kv{ ExplicitDefaultConstruct() };

	// Can't use, tries to copy and mutate
	//std::vector<int, IntAllocator> vector{ alloc_i };
	//std::deque<int, IntAllocator> deque{ alloc_i };
	//std::list<int, IntAllocator> list{ alloc_i };
	//std::set<int, std::less<int>, IntAllocator> set{ alloc_i };
	//std::map<int, int, std::less<int>, KVAllocator> set{ alloc_kv };
	//std::unordered_set<int, std::hash<int>, std::equal_to<int>, IntAllocator> set{ alloc_i };
	//std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, KVAllocator> set{ alloc_kv };
}

///////////////////////////////////////////////////////////////////////////////
}
