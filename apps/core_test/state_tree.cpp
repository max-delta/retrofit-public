#include "stdafx.h"

#include "core_state/StateTree.h"

#include "core_allocate/LinearAllocator.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

TEST( StateTree, Basic )
{
	using Tree = StateTree<int, 4>;
	using Stream = Tree::StreamType;
	state::VariableIdentifier identifier;

	alloc::AllocatorT<alloc::LinearAllocator<512>> allocator{ ExplicitDefaultConstruct() };

	Tree tree;
	ASSERT_EQ( tree.GetStream( identifier ), nullptr );
	ASSERT_EQ( tree.GetMutableStream( identifier ), nullptr );

	WeakPtr<Stream> const stream = tree.GetOrCreateStream( identifier, allocator );
	ASSERT_NE( stream, nullptr );
	ASSERT_EQ( tree.GetStream( identifier ), stream );
	ASSERT_EQ( tree.GetMutableStream( identifier ), stream );

	WeakPtr<Stream> const recreate = tree.GetOrCreateStream( identifier, allocator );
	ASSERT_EQ( recreate, stream );
	ASSERT_EQ( tree.GetStream( identifier ), stream );
	ASSERT_EQ( tree.GetMutableStream( identifier ), stream );
}

///////////////////////////////////////////////////////////////////////////////
}}
