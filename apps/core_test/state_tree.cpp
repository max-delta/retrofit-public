#include "stdafx.h"

#include "core_state/StateTree.h"

#include "core_allocate/LinearAllocator.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

TEST( StateTree, Basic )
{
	using Tree = StateTree<int, 4>;
	using Stream = Tree::StreamType;
	static constexpr char kIdentifier[] = "";

	alloc::AllocatorT<alloc::LinearAllocator<512>> allocator{ ExplicitDefaultConstruct() };

	Tree tree;
	ASSERT_EQ( tree.GetStream( kIdentifier ), nullptr );
	ASSERT_EQ( tree.GetMutableStream( kIdentifier ), nullptr );

	WeakPtr<Stream> const stream = tree.GetOrCreateStream( kIdentifier, allocator );
	ASSERT_NE( stream, nullptr );
	ASSERT_EQ( tree.GetStream( kIdentifier ), stream );
	ASSERT_EQ( tree.GetMutableStream( kIdentifier ), stream );

	WeakPtr<Stream> const recreate = tree.GetOrCreateStream( kIdentifier, allocator );
	ASSERT_EQ( recreate, stream );
	ASSERT_EQ( tree.GetStream( kIdentifier ), stream );
	ASSERT_EQ( tree.GetMutableStream( kIdentifier ), stream );
}

///////////////////////////////////////////////////////////////////////////////
}}
