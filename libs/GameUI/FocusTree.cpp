#include "stdafx.h"
#include "FocusTree.h"

#include "GameUI/FocusTarget.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

FocusTree::ConstNodeStack FocusTree::GetCurrentFocusStack() const
{
	ConstNodeStack retVal;
	WeakPtr<Node const> traverse = mRootNode;
	while( traverse != nullptr )
	{
		Node const& node = *traverse;

		if( IsValidForFocus( node ) )
		{
			retVal.emplace_back( traverse );
		}

		if( ShouldTraverseForFocus( node ) )
		{
			traverse = node.mFavoredChild;
		}
	}

	return retVal;
}



WeakPtr<FocusTree::Node const> FocusTree::GetCurrentFocus() const
{
	WeakPtr<Node const> retVal;
	WeakPtr<Node const> traverse = mRootNode;
	while( traverse != nullptr )
	{
		Node const& node = *traverse;

		if( IsValidForFocus( node ) )
		{
			retVal = traverse;
		}

		if( ShouldTraverseForFocus( node ) )
		{
			traverse = node.mFavoredChild;
		}
	}

	return retVal;
}



bool FocusTree::IsCurrentFocus( Node const& node ) const
{
	Node const* currentFocus = GetCurrentFocus();
	return &node == currentFocus;
}



bool FocusTree::IsCurrentFocus( FocusTarget const& target ) const
{
	Node const* currentFocus = GetCurrentFocus();
	if( currentFocus == nullptr )
	{
		return false;
	}
	FocusTarget const* currentTarget = currentFocus->mFocusTarget;
	RF_ASSERT( currentFocus != nullptr );
	return &target == currentTarget;
}



bool FocusTree::IsValidForFocus( Node const& node )
{
	FocusTarget const* const target = node.mFocusTarget;
	if( target == nullptr )
	{
		return false;
	}

	if( target->mIsFocusable == false )
	{
		return false;
	}

	return true;
}



bool FocusTree::ShouldTraverseForFocus( Node const& node )
{
	FocusTarget const* const target = node.mFocusTarget;
	if( target == nullptr )
	{
		return false;
	}

	if( target->mIsFocusable == false )
	{
		return false;
	}

	if( target->mPropogatesFocus == false )
	{
		return false;
	}

	return true;
}



WeakPtr<FocusTree::Node const> FocusTree::FindNode( FocusTarget const& target ) const
{
	for( BackingNodeStorage::value_type const& nodeRef : mBackingNodeStorage )
	{
		WeakPtr<Node const> possibleReturn = nodeRef;
		Node const* const node = possibleReturn;
		RF_ASSERT( node != nullptr );
		if( node->mFocusTarget == &target )
		{
			return possibleReturn;
		}
	}
	return nullptr;
}



bool FocusTree::CreateNewChild( Node const& parentNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( parentNode.mFavoredChild != nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	Node& node = const_cast<Node&>( parentNode );
	UniquePtr<Node> newNode = DefaultCreator<Node>::Create();
	newNode->mFocusTarget = newTarget;

	node.mFavoredChild = newNode;

	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return true;
}



bool FocusTree::CreateNewSiblingAfter( WeakPtr<Node> previousNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( previousNode == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	Node& node = *previousNode;
	UniquePtr<Node> newNode = DefaultCreator<Node>::Create();
	newNode->mFocusTarget = newTarget;

	// A' - B'' - C
	WeakPtr<Node> next = node.mNextSibling; // C
	node.mNextSibling = newNode; // A -> B
	newNode->mPreviousSibling = previousNode; // A <- B
	if( next != nullptr )
	{
		newNode->mNextSibling = next; // B -> C
		next->mPreviousSibling = newNode; // B <- C
	}

	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return true;
}



bool FocusTree::CreateNewSiblingBefore( WeakPtr<Node> nextNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( nextNode == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	Node& node = *nextNode;
	UniquePtr<Node> newNode = DefaultCreator<Node>::Create();
	newNode->mFocusTarget = newTarget;

	// A - B'' - C'
	WeakPtr<Node> previous = node.mPreviousSibling; // A
	node.mNextSibling = newNode; // B <- C
	newNode->mPreviousSibling = nextNode; // B -> C
	if( previous != nullptr )
	{
		newNode->mPreviousSibling = previous; // A <- B
		previous->mNextSibling = newNode; // A -> B
	}

	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return true;
}



void FocusTree::TrimDeadLinks()
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}}
